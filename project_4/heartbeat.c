/* heartbeat.c 
	 Date: Dec 2, 2015
	 Bare minimum tcp/ip server, listening to heartbeat requests from the selector.
	 Updated Date : Dec 2, 2015.
*/

#include "keyval.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[]){
				int port = 20000;
				if(argc != 2){
								printf("Error\nUsage: hearbeat port\n");
								exit(1);
				}	
				int sockfd, newsock, recv_bytes ;
				struct sockaddr_in server_addr, client_addr;
				bool val = true;

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
				fflush(stdout);
				while(1){
								/* accept the connection */
								int sin_size = sizeof(struct sockaddr_in);
								newsock = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
								printf("Accepted Request\n");
								close(newsock);
				}
				close(sockfd);
				return 0;
}
