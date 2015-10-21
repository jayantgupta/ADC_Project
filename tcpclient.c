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
#include<stdbool.h>
int main(int argc, char *argv []){
    if(argc != 3){
	    printf("Error\nUsage ./tcpclient.exec host-name/host-IP port\n");
	    exit(1);
    }
    printf("%s %s\n",argv[1], argv[2]);
    int sockfd, recv_bytes;
    char send_buff[1024],recv_buff[1024];
    struct hostent *host;
    struct sockaddr_in server_addr;
    host = gethostbyname(argv[1]);
    host = gethostbyname("127.0.0.1");
    const char *log_filename = "tcp_client.log";
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
    /* message to send to the server */
    char *FORMAT = "Message Format\nGET:<key>\nPUT:<key>:<ue>\nDELETE:<key>\n";
    printf("%sEnter the message: ", FORMAT);
    scanf("%s",send_buff);
   // fgets(send_buff,1024,stdin);
    /* send message to the server */
    send(sockfd,send_buff,strlen(send_buff),0);
    callLog("Message sent to server\n", log_filename);
    while(1)
    {
        recv_bytes=recv(sockfd,recv_buff,1024,0);
        callLog("Message received from server\n", log_filename);
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
        	send(sockfd,send_buff,strlen(send_buff), 0);
	}
        else
        {
            send(sockfd,send_buff,strlen(send_buff), 0);
            close(sockfd);
            break;
        }
    }
    return 0;
}
