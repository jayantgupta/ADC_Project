/* tcpclient.c */
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

int main(int argc, char *argv []){
    // Check for Arguments
    if(argc != 3){
	    printf("Error\nUsage ./tcpclient.exec host-name/host-IP port\n");
	    exit(1);
    }
    printf("%s %s\n",argv[1], argv[2]);
    // Create Variables.
    int sockfd, recv_bytes;
    char send_buff[1024],recv_buff[1024];
    struct sockaddr_in server_addr;
    struct hostent *host = gethostbyname(argv[1]);
    const char *log_filename = "tcp_client.log";
    
    fd_set myset;
    struct timeval tv;
    /* creating a socket */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket");
        callLog("error in creating a socket\n", log_filename);
        exit(1);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(server_addr.sin_zero),8);
    /*connect the socket to address of server*/
    if (connect(sockfd, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) == -1)
    {
        perror("Failed to connect");
        callLog("Failed to connect socket to the address of the server\n", log_filename);
        exit(1);
    }
    
    /* Creating Time out values */
    tv.tv_sec = 2;  // seconds
    tv.tv_usec = 0; // microseconds
    FD_ZERO(&myset);
    FD_SET(sockfd, &myset);
    
    while(1)
    {
       /* Get message to send to the server */
        const char *FORMAT = "Message Format\nGET:<key>\nPUT:<key>:<ue>\nDELETE:<key>\nQ to quit\n";
        printf("%sEnter the message: ", FORMAT);
        scanf("%s",send_buff);
				int send_count = 1;
				int status ;
        /* send message to the server */
        if (strcmp(send_buff , "Q") != 0){
								send(sockfd,send_buff,strlen(send_buff), 0);
								callLog("Message sent to server\n", log_filename);
								while((status=select(sockfd + 1, &myset, NULL, NULL, &tv)) <= 0 && send_count < 5){
												send(sockfd, send_buff, strlen(send_buff), 0);
												printf("Re-sending Request to Server %d time\n",send_count);
												send_count++;
												
								}
								if(status > 0){
												recv_bytes = recv(sockfd, recv_buff, 1024, 0);
												callLog("Message received from server\n", log_filename);
												recv_buff[recv_bytes] = '\0';
												printf("Message from server : %s \n" , recv_buff);
								}
								else if(status <= 0 && send_count == 5){
												printf("Resend failed\n");
												callLog("Resend failed\n", log_filename);
								}
				}
        else{
            send(sockfd,send_buff,strlen(send_buff), 0);
            close(sockfd);
            break;
        }
    }
    return 0;
}
