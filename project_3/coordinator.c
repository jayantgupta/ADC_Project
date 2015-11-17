/*
	tcpclient.c 
	TOADD : Timeout.
	Initial version by Alaka Sukumar
	Edited by Jayant Gupta to serve
	the purpose of 2-phase commit. 
	This will act as a coordinator, and will end the request 
	for Commits to all the servers, including itself.
	Date. Nov 13, 2015.
*/
#include "coordinator.h"

int ack_count;
int go_ack_count;
pthread_mutex_t thread_lock;
const int count = 2;

void get_and_add_ack(){
			pthread_mutex_lock(&thread_lock);
			ack_count++;
			pthread_mutex_unlock(&thread_lock);			
}

void get_and_add_go_ack(){
			pthread_mutex_lock(&thread_lock);
			go_ack_count++;
			pthread_mutex_unlock(&thread_lock);
}

void *two_pc_protocol(void *addr){
		struct host_addr{
			char *IP;
			char *port;
			char rqst[1024];
		}* t_addr;
		t_addr = (struct host_addr *)addr;
		char *IP = t_addr->IP;
		char *port = t_addr->port;
		char *request = t_addr->rqst;
		printf("Connect to %s %s with request %s\n", IP, port, request);

    int sockfd, recv_bytes;
    char send_buff[1024], recv_buff[1024];
    struct sockaddr_in server_addr;
    struct hostent *host = gethostbyname(IP);
 
		fd_set myset;
    struct timeval tv;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("Socket");
				printf("Unable to create socket\n");
//        callLog("error in creating a socket\n", log_filename);
        exit(1);
    }
    /* creating a socket */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port));
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(server_addr.sin_zero),8);
    printf("Connection.... init\n");
		/*connect the socket to address of server*/
    if (connect(sockfd, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) == -1)
    {
        perror("Failed to connect");
				printf("connection failed\n");
//        callLog("Failed to connect socket to the address of the server\n", log_filename);
        exit(1);
    }

		/* Get message to send to the server */
		int send_count = 1;
		int status ;
		/* send message to the server */
		send(sockfd, request, strlen(request), 0);
		recv_bytes = recv(sockfd, recv_buff, 1024, 0);
		printf("%s--%s-- ", port, recv_buff);
		if(strcmp(recv_buff, "ACK") == 0){
			get_and_add_ack();
			printf("Here\n");
		}
		printf("%s : %d\n", port, ack_count);
		while(ack_count < count){
			//spin;
		}
		printf("%s : %d\n", port, ack_count);
		send(sockfd, "GO", strlen("GO"), 0);
		recv_bytes = recv(sockfd, recv_buff, 1024, 0);
		if(strcmp(recv_buff,"ACK") == 0){
			printf("Now Here\n");
			get_and_add_go_ack();
		}
		while(go_ack_count < count){
			//spin
		}
		printf("%s : %d\n", port, go_ack_count);
		close(sockfd);
		pthread_exit(0);
		return ;
}

// Coordinator.
// This will be called by rpc_kev_server
bool run(char request[1024]){
		printf("%s\n", request);
		int *status[count];
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		printf("%s\n", request);
		int i, ret; // Count of Peers.
		char *ip[] = {"localhost", "localhost"}; // add ip here.
		char *ports[] = {"10000", "10001"};  // add ports here.
		struct host_addr
		{
				char *IP;
				char *port;
				char rqst[1024];
		};
		struct host_addr **addr = (struct host_addr **)malloc(sizeof(struct host_addr*)*count);
		pthread_t thread_array[count];
		ack_count = 0;
		go_ack_count = 0;
		for(i = 0 ; i < count ; i++){
			addr[i] = (struct host_addr *)malloc(sizeof(struct host_addr));
			addr[i] -> IP = ip[i];
			addr[i] -> port = ports[i];
			strcpy(addr[i] -> rqst,  request);
			if((ret = pthread_create(&thread_array[i], NULL, two_pc_protocol, (void *)addr[i])) == -1){
				printf("Thread creation failed with return code: %d", ret);
				exit(ret);
			}
		}
		for(i = 0 ; i < count ; i++){
			pthread_join(thread_array[i], (void **) &(status[i]));
		}
//    pthread_exit(0);
		return true;
}
