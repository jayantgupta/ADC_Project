/* udpserver.c */

#include "keyval.h"
#include "logfile.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	/*
		Setting up the key-value directory.
	*/
	if(argc != 2){
		printf("Error \n Usage ./udpserver.exec port\n");
		exit(1);
	}
	const char *name = "word_list";
//	init_dict(name);	
	int sockfd, newsock, recv_bytes ;
	bool val = true;
	struct sockaddr_in server_addr, client_addr;
	int sin_size;
  const char *log_filename = "udp_server.log";
       
	/* creating a socket */
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Error opening socket");
		exit(1);
	}
	if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(int)) == -1)
	{
		perror("Error in setting socket opt");
		exit(1);
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[1]));
	server_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(server_addr.sin_zero),8);
	/* bind the socket to an address */
	if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("Error in binding");
		exit(1);
	}
	
		char send_buff [1024] , recv_buff[1024];
		sin_size = sizeof(client_addr);
                printf("Server Listening ..\n");
		
		while(1)
		{
			/*receive message from client */
			recv_bytes = recvfrom(sockfd, recv_buff, 1024, 0,(struct sockaddr *)&client_addr,&sin_size);
			recv_buff[recv_bytes]= '\0';
			printf("Message from client : |%s| of size : %d\n" , recv_buff, recv_bytes);
			if (strcmp(recv_buff , "q") == 0 || strcmp(recv_buff , "Q") == 0)
			{
				//printf("Closing Connection\n");
        callLog("Closing Connnection \n", log_filename);
				sendto(sockfd, "Q", 2, 0,(struct sockaddr *)&client_addr,sizeof(struct sockaddr));
				close(sockfd);
				break;
			}
			fflush(stdout);

			/* code to validate message from client*/
			const char s[2] = ":";
			char *token;
			/* get the first token */
			token = strtok(recv_buff, s);            
			char * allTokens[4];             
			int i=0;
			while( token != NULL && i < 3) // change by me. 
			{  
				allTokens[i] = token;
				token = strtok(NULL, s);
//				printf("token : %s",allTokens[i]);
				i++;
			}
			if(token != NULL && i >= 3){
				//printf("Error in syntax\n");
        callLog("Error in syntax \n", log_filename);
				sendto(sockfd, "ERROR", 6, 0,(struct sockaddr *)&client_addr,sin_size);
				break;
			}
			if(strcmp(allTokens[0],"GET") == 0){
				printf("Inside GET\n");
				callLog("Called method GET \n", log_filename);
				int key = atoi(allTokens[1]);
				char *getValue = GET(key);
				sendto(sockfd, getValue, strlen(getValue), 0,(struct sockaddr *)&client_addr,sin_size);             
				
			}
			else if(strcmp(allTokens[0],"PUT") == 0){
				callLog("Called method PUT \n", log_filename);
				bool putValue = PUT(atoi(allTokens[1]), allTokens[2]); 
				if(putValue){
					sendto(sockfd, "Successfully updated", 21, 0,(struct sockaddr *)&client_addr,sin_size); 
          callLog("Successfully Updated\n", log_filename);
				}

				else{
					sendto(sockfd, "Error in updating",18, 0,(struct sockaddr *)&client_addr,sin_size);
          callLog("Error in updating \n", log_filename);
				}

			}
			else if(strcmp(allTokens[0],"DELETE") == 0){
				callLog("Called method DELETE \n", log_filename);
				bool delValue = DELETE(atoi(allTokens[1]));
				if(delValue){
					sendto(sockfd, "Successfully deleted",21, 0,(struct sockaddr *)&client_addr,sin_size); 
          callLog("Successfully Deleted \n", log_filename);
				}
				else{
					sendto(sockfd, "Error in deleting",18, 0,(struct sockaddr *)&client_addr,sin_size);
          callLog("Error in Deleting \n", log_filename);
				}                
			}
			else{
				printf("Error in syntax : Improper Command");
        callLog("Error in syntax : Improper Command \n", log_filename);
				sendto(sockfd, "ERROR in CMD", 13, 0,(struct sockaddr *)&client_addr,sin_size);
			}
		//break; // Putting this break right now to server one client only.
		}
  printf("Connection Served\n");
  callLog("Connection served \n", log_filename);
	close(sockfd);
	return 0;
}

