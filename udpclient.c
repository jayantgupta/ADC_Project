/* udpclient.c */
#include "logfile.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/select.h>

int main(int argc, char *argv[]){
	  // Check for required number of arguments.
    if(argc != 3){
	    printf("Error\nUsage ./udpclient.exec host-name/host-IP port\n");
	    exit(1);
    }
		// Create Variables
    int sockfd, recv_bytes;
    char send_buff[1024],recv_buff[1024];
    struct hostent *host;
    struct sockaddr_in server_addr;
    host = gethostbyname(argv[1]);
    int sin_size;
    const char *log_filename = "udp_client.log";
    
		fd_set myset;
		struct timeval tv;
		/* creating a socket */
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("Socket");
        exit(1);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(server_addr.sin_zero),8);

		/* Creating Time Out Values */
		tv.tv_sec = 2 ; //seconds
		tv.tv_usec = 0; //microseconds
		FD_ZERO(&myset);
		FD_SET(sockfd, &myset);
  
    while(1)
    {
    		/* message to send to the server */
    		const char *FORMAT = "Message Format\nGET:<key>\nPUT:<key>:<value>\nDELETE:<key>\nQ to quit.\n";
    		printf("%sEnter the message: ", FORMAT);
    		scanf("%s",send_buff);
				int send_count = 1;
				int status ;
        if (strcmp(send_buff , "Q") != 0){
        	sendto(sockfd,send_buff,strlen(send_buff), 0,(struct sockaddr *)&server_addr,sizeof(struct sockaddr));
					//sendto(sockfd,send_buff,strlen(send_buff),0,(struct sockaddr *)&server_addr,sizeof(struct sockaddr));
					callLog("Message Sent to server\n", log_filename);
					while((status=select(sockfd + 1, &myset, NULL, NULL, &tv))<=0 && send_count < 5){
        		sendto(sockfd,send_buff,strlen(send_buff), 0,(struct sockaddr *)&server_addr,sizeof(struct sockaddr));
						printf("Re-sending Request to Server %d time\n", send_count);
						send_count++;
					}
					if(status > 0){
									sin_size = sizeof(struct sockaddr_in);
									recv_bytes=recvfrom(sockfd,recv_buff,1024,0,(struct sockaddr *)&server_addr,&sin_size);
									callLog("Received Message from Server\n", log_filename);
									recv_buff[recv_bytes] = '\0';
									printf("Message from server : %s \n" , recv_buff);
					}
					else if(status <= 0 && send_count == 5){
									printf("Resend failed\n");
									callLog("Resend failed\n", log_filename);
					}
	 			}
        else
        {
            sendto(sockfd,send_buff,strlen(send_buff), 0,(struct sockaddr *)&server_addr,sizeof(struct sockaddr));
            close(sockfd);
            break;
        }
    }
    return 0;
}
