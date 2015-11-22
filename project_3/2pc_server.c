/* tcpserver.c 
	 Serving 2-phase commit requests amongst various servers
	 Initial Version : Alaka Sukumar
	 2-phase commit version : Jayant Gupta
 	 Updated Date : November 13, 2015
*/

#include "keyval.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

const char *GET_FILE = "GET_buffer";
char *exec_rqst(char *);

int main(int argc, char *argv[]){
	if(argc != 2){
		printf("Error\nUsage ./2pc_server.exec port\n");
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
//	callLog("Server listening \n", log_filename);
	fflush(stdout);
	while(1){
		char send_buff [1024] , recv_buff[1024], request[1024];
		sin_size = sizeof(struct sockaddr_in);
		/* accept the connection */
		newsock = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
		printf("Accepted Request\n");
		// Serve first request.
		recv_bytes = recv(newsock, recv_buff, 1024, 0);
		recv_buff[recv_bytes] = '\0';
		printf("%s\n", recv_buff);
		strcpy(request, recv_buff); // Store request to be processed later.
		send(newsock, "ACK", strlen("ACK"), 0);	// Send back the Acknowledgement.
		recv_bytes = recv(newsock, recv_buff, 1024, 0);
		recv_buff[recv_bytes] = '\0'; // Wait for the final go. TODO add timeout.
		send(newsock, "ACK", strlen("ACK"), 0);
		if(strcmp(recv_buff, "GO") == 0){
			exec_rqst(request); // Execute the request.
			printf("Request : %s\n", request);
		}
		close(newsock);
	}
	close(sockfd);
	return 0;
}

// Takes the first request and responds as ACK.
// Assuming all the errors are checked at the coordinator-server
char * exec_rqst(char * request){
		printf("Executing Request : %s\n", request);
		/* code to validate the request*/
		const char s[2] = ":";
		char *token;
		/* get the first token */
		token = strtok(request, s);            
		char allTokens[3][26];             
		int i = 0;
		while( token != NULL && i < 3)   
		{ 
					 printf("%s\n", token);	
						strcpy(allTokens[i],token);
						token = strtok(NULL, s);
						i++;
		}
		if(strcmp(allTokens[0],"GET") == 0){ 
//					callLog("Called method GET \n", log_filename);
						printf("Called method GET\n");
						int key = atoi(allTokens[1]);
						char *getValue = _GET(key);
//					send(newsock, getValue, strlen(getValue), 0);    
						FILE *fp = fopen(GET_FILE, "w");
						fputs(getValue, fp);
						fclose(fp);
		} 
		else if(strcmp(allTokens[0],"PUT") == 0){
//					callLog("Called method PUT \n", log_filename);
						printf("Inside PUT %s %s\n", allTokens[1], allTokens[2]);
						bool putValue = _PUT(atoi(allTokens[1]), allTokens[2]); 
						if(putValue){
//							callLog("Successfully Updated\n", log_filename);
								return "COMMIT";
						}

						else{
//						callLog("Error in updating \n", log_filename);
							return "PUT_ERROR";
						}

		}
		else if(strcmp(allTokens[0],"DELETE") == 0){
//					callLog("Called method DELETE \n", log_filename);
						printf("Inside DELETE\n");
						bool delValue = _DELETE(atoi(allTokens[1]));
						if(delValue){
//							callLog("Successfully Deleted \n", log_filename);
							return "COMMIT";
						}
						else{
//							callLog("Error in Deleting \n", log_filename);
							return "DELETE_ERROR";
						}                
		}
		return "UN_KNOWN_REQUEST";
}
