/* tcpclient.c */
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
int main()
{
    int sockfd, recv_bytes;
    char send_buff[1024],recv_buff[1024];
    struct hostent *host;
    struct sockaddr_in server_addr;
    host = gethostbyname("127.0.0.1");
    /* creating a socket */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket");
        exit(1);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9006);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(server_addr.sin_zero),8);
    /*connect the socket to address of server*/
    if (connect(sockfd, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) == -1)
    {
        perror("Failed to connect");
        exit(1);
    }
    /* message to send to the server */
    char *FORMAT = "Message Format\nGET:<key>\nPUT:<key>:<value>\nDELETE:<key>\n";
    printf("%sEnter the message: ", FORMAT);
    scanf("%s",send_buff);
   // fgets(send_buff,1024,stdin);
    /* send message to the server */
    send(sockfd,send_buff,strlen(send_buff),0);
    while(1)
    {
        recv_bytes=recv(sockfd,recv_buff,1024,0);
        recv_buff[recv_bytes] = '\0';
        if (strcmp(recv_buff , "q") == 0 || strcmp(recv_buff , "Q") == 0)
        {
            close(sockfd);
            break;
        }
        else
        printf("Message from server : %s \n" , recv_buff);
        printf("%sEnter another message or Q to quit: ",FORMAT);
	// - gets(send_buff);
        //+ 
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
