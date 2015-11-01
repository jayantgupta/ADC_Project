/* tcpserver.c */

#include "keyval.h"
#include "logfile.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[]){
	if(argc != 2){
		printf("Error\nUsage ./tcpserver.exec port\n");
		exit(1);
	}	
	/*
		Setting up the key-value directory.
	*/
	const char *name = "word_list";
	//init_dict(name);	
	int sockfd, newsock, recv_bytes ;
	bool val = true;
	struct sockaddr_in server_addr, client_addr;
	int sin_size;
  const char *log_filename = "tcp_server.log";
       
	/* creating a socket */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
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
	if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("Error in binding");
		exit(1);
	}
	/* listen for connections*/
	if (listen(sockfd, 5) == -1)
	{
		perror("Listen");
		exit(1);
	}
        /* logging server listening */
        printf("Server Listening ..\n");
	callLog("Server listening \n", log_filename);
	fflush(stdout);
	while(1)
	{
		char send_buff [1024] , recv_buff[1024];
		sin_size = sizeof(struct sockaddr_in);
		/* accept the connection */
		newsock = accept(sockfd, (struct sockaddr *)&client_addr,&sin_size);
		while(1)
		{
			/*receive message from client */
			recv_bytes = recv(newsock, recv_buff, 1024, 0);
			recv_buff[recv_bytes]= '\0';
			printf("Message from client : |%s| of size : %d\n" , recv_buff, recv_bytes);
			if (strcmp(recv_buff , "q") == 0 || strcmp(recv_buff , "Q") == 0)
			{
				//printf("Closing Connection\n");
                                callLog("Closing Connnection \n", log_filename);
				send(newsock, "Q", 2, 0);
				close(newsock);
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
				i++;
			}
			if(token != NULL && i >= 3){
				//printf("Error in syntax\n");
                                callLog("Error in syntax \n", log_filename);
				send(newsock, "ERROR", 6, 0);
				break;
			}
			if(strcmp(allTokens[0],"GET") == 0){
				callLog("Called method GET \n", log_filename);
				int key = atoi(allTokens[1]);
				char *getValue = GET(key);
				send(newsock, getValue, strlen(getValue), 0);             
			}
			else if(strcmp(allTokens[0],"PUT") == 0){
				callLog("Called method PUT \n", log_filename);
				bool putValue = PUT(atoi(allTokens[1]), allTokens[2]); 
				if(putValue){
					send(newsock, "Successfully updated", 21, 0); 
                                        callLog("Successfully Updated\n", log_filename);
				}

				else{
					send(newsock, "Error in updating",18, 0);
                                        callLog("Error in updating \n", log_filename);
				}

			}
			else if(strcmp(allTokens[0],"DELETE") == 0){
				callLog("Called method DELETE \n", log_filename);
				bool delValue = DELETE(atoi(allTokens[1]));
				if(delValue){
					send(newsock, "Successfully deleted",21, 0); 
                                        callLog("Successfully Deleted \n", log_filename);
				}
				else{
					send(newsock, "Error in deleting",18, 0);
                                        callLog("Error in Deleting \n", log_filename);
				}                
			}
			else{
				printf("Error in syntax : Improper Command");
                                callLog("Error in syntax : Improper Command \n", log_filename);
				send(newsock, "ERROR in CMD", 13, 0);
			}
		}
		printf("Connection Served\n");
                callLog("Connection served \n", log_filename);
		break; // Putting this break right now to server one client only.
	}
	close(sockfd);
	return 0;
}

