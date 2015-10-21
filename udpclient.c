/* udpclient.c */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include<stdbool.h>
#include "logfile.h"
int main(int argc, char *argv[])
{
    if(argc != 3){
	    printf("Error\nUsage ./udpclient.exec host-name/host-IP port\n");
	    exit(1);
    }
    int sockfd, recv_bytes;
    char send_buff[1024],recv_buff[1024];
    struct hostent *host;
    struct sockaddr_in server_addr;
    host = gethostbyname(argv[1]);
    int sin_size;
    const char *log_filename = "udp_client.log";
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
  
    /* message to send to the server */
    char *FORMAT = "Message Format\nGET:<key>\nPUT:<key>:<value>\nDELETE:<key>\n";
    printf("%sEnter the message: ", FORMAT);
    scanf("%s",send_buff);     
    sendto(sockfd,send_buff,strlen(send_buff),0,(struct sockaddr *)&server_addr,sizeof(struct sockaddr));
    callLog("Message Sent to server\n", log_filename);
    while(1)
    {
        sin_size = sizeof(struct sockaddr_in);
        recv_bytes=recvfrom(sockfd,recv_buff,1024,0,(struct sockaddr *)&server_addr,&sin_size);
        callLog("Received Message from Server\n", log_filename);
        recv_buff[recv_bytes] = '\0';
        if (strcmp(recv_buff , "q") == 0 || strcmp(recv_buff , "Q") == 0)
        {
            close(sockfd);
            break;
        }
        else
        printf("Message from server : %s \n" , recv_buff);
        printf("%sEnter another message or Q to quit: ",FORMAT);
	 
	scanf("%s",send_buff);
        if (strcmp(send_buff , "Q") != 0){
        	sendto(sockfd,send_buff,strlen(send_buff), 0,(struct sockaddr *)&server_addr,sizeof(struct sockaddr));
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
