/*
	# selector.c
	Author: Jayant Gupta	
	Date: Dec 2, 2015
	This code has the following functionality.
	Checks the heartbeat of all the local servers at every second.
  Elect the leader at the start or when the current leader fails. 
*/

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
#include <pthread.h>
#include<time.h>

/*
	Leader election is done either at the initializing of 
	the system or when the current leader fails.
	Leader Selection ::
	Selects the first node alive starting from n01 to n09.
	If n01 fails select next in the sequence, if n02 is 
	the leader and n01 wakes up, then n02 will still 
	remain the leader. 
*/

int tcp_connect(char *IP, char *port);
char *heartbeat(char **ips, char **ports, int len, char *leader);
char *leader_election();

int main(int argc, char** argv){
	char *ips[] = {"localhost", "localhost", "localhost", "localhost"};
	char *ports[] = {"10000", "10001", "10002", "10003"};
	int len = (sizeof(ips) / sizeof(ips[0])) ;

	char * leader = leader_election(ips, ports);
	printf("here\n");
	while(true && leader != NULL){
		printf("Current Leader : %s\n", leader);
		leader = heartbeat(ips, ports, len, leader);
		sleep(1);
	}
	return 0;	
}

char *leader_election(char **ips, char **ports, int len){
	printf("Electing Leader\n");
	// Priority is given to n01 > n02 and so on.
	int i;
	for(i = 0 ; i < len ; i++){
		int sockfd = tcp_connect(ips[i], ports[i]);
		if(sockfd == -1) continue;
		else{
			close(sockfd);
			return ports[i];
		}
	}	
	printf("FATAL error; No node alive\n");
	return NULL;
}

int tcp_connect(char *IP, char *port){
				int sockfd;
				struct sockaddr_in server_addr;
				struct hostent *host = gethostbyname(IP);
				if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
								perror("Socket");
								printf("Unable to create socket\n");
								exit(1);
				}   
				/* creating a socket */
				server_addr.sin_family = AF_INET;
				server_addr.sin_port = htons(atoi(port));
				server_addr.sin_addr = *((struct in_addr *)host->h_addr);
				bzero(&(server_addr.sin_zero),8);
//				printf("Connection.... init\n");
				/*connect the socket to address of server*/
				if (connect(sockfd, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) == -1) 
				{   
								perror("Failed to connect");
								return -1; 
				}   
				return sockfd;
}

char *heartbeat(char **ips,
								char **ports,
								int len,
								char *leader){
				printf("sending heartbeat\n");
				int i, sockfd;
				int alive = 0;
				for(i = 0 ; i < len ; i++){
								sockfd = tcp_connect(ips[i], ports[i]);
								printf("sockfd : %d\n",sockfd);
								if(sockfd == -1){ // node failed
												if(strcmp(leader, ports[i]) == 0){ // Leader failed.
																leader = leader_election(ips, ports, len);
												}
												else{
																printf("node %s:%s has failed\n",ips[i], ports[i]);
												}
								}
								else{
												close(sockfd);
												alive++;
												printf("node %s:%s is alive\n", ips[i], ports[i]);
								} // check next node.
				}
				printf("Heartbeat check #alive-nodes:%d #dead-nodes:%d\n", alive, len - alive);
				return leader;
}
