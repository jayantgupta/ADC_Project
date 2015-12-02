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

//TODO add bootstrap of the data, the function to be added 
//     in 2pc_server.c (acceptor)
//TODO add Election Algorithm
#include "coordinator.h"
#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

int ack_count;
int go_ack_count;
pthread_mutex_t thread_lock;
const int count = 2; // Number of nodes.
char *leader;

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
    printf("Connection.... init\n");
		/*connect the socket to address of server*/
    if (connect(sockfd, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) == -1)
    {
        perror("Failed to connect");
				printf("connection failed\n");
				return -1;
    }
		return sockfd;
}
void *hearbeat(void *addr_list){
	struct host_list{
		char **ips;
		char **ports;
		int len;
	}* p_addr_list;
	p_addr_list = (struct host_list*)addr_list;
	char **ips = p_addr_list->ips;
	char **ports = p_addr_list->ports;
	int len = p_addr_list->len;
	int i, sockfd;
	char *ping = "ping";
	int alive = 0;
	for(i = 0 ; i < len ; i++){
		sockfd = tcp_connect(ips[i], ports[i]);	
		if(sockfd == -1){ // node failed
			if(strcmp(leader, ips[i]) == 0){ // Leader failed.
				leader_election();						
			}
			else{
				printf("node %s:%s failed\n",ips[i], ports[i]);
			}
		}
		else{
			alive++;
			printf("node %s:%s alive", ips[i], ports[i]);
		} // check next node.
	}
	printf("Heartbeat check #alive-nodes:%d #dead-nodes:%d", alive, len - alive);
}

void *two_pc_protocol(void *addr){
		struct host_addr{
			char *IP;
			char *port;
			char rqst[1024];
			bool read_flag;
		}* t_addr;
		t_addr = (struct host_addr *)addr;
		char *IP = t_addr->IP;
		char *port = t_addr->port;
		char *request = t_addr->rqst;
		bool read_flag = t_addr->read_flag;
		printf("Connect to %s %s with request %s\n", IP, port, request);

    int recv_bytes, status;
    char send_buff[1024], recv_buff[1024];
		int send_count = 1;
		int sockfd = tcp_connect(IP, port);
		send(sockfd, request, strlen(request), 0); /* send message to the server */
		recv_bytes = recv(sockfd, recv_buff, 1024, 0); /* recv messages from 2pc_server*/
		printf("Received %s from %s:%s.\n", recv_buff, IP, port);
		if(strcmp(recv_buff, "ACK") == 0){
						get_and_add_ack();
						printf("Ack Received.\n");
		}
		while(ack_count < count && !read_flag){
			//spin;
		}
		send(sockfd, "GO", strlen("GO"), 0); // Sending GO message.
		/* Adding Timeout */
		recv_bytes = recv(sockfd, recv_buff, 1024, 0);
		printf("Received %s from %s:%s.\n", recv_buff, IP, port);
		if(strcmp(recv_buff,"ACK") == 0){
			get_and_add_go_ack();
			printf("GO_ACK received.");
		}
		while(go_ack_count < count && !read_flag){
			//spin
		}
		close(sockfd);
		pthread_exit(0);
		return NULL ;
}


// Coordinator.
// This will be called by rpc_kev_server
bool run(char request[1024], bool read_flag){
		printf("%s\n", request);
		int *status[count];
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		printf("%s\n", request);
		int i, ret; // Count of Peers.
//		char *ip[] = {"n01", "n02", "n03", "n04", "n05", "n06", "n07", "n08", "n09"}; // add ip here.
//		char *ports[] = {"10000", "10000", "10000", "10000", "10000", "10000", "10000", "10000", "10000"};  // add ports here.
		char *ip[] = {"localhost", "localhost"}; // add ip here.
		char *ports[] = {"10000", "10001"};  // add ports here.
		if((sizeof(ip) / sizeof(ip[0])) != count){
			printf("Incorrect global count value!!\n");
		}
		struct host_addr
		{
				char *IP;
				char *port;
				char rqst[1024];
				bool read_flag;
		};
		struct host_addr **addr = (struct host_addr **)malloc(sizeof(struct host_addr*)*count);
		pthread_t thread_array[count];
		ack_count = 0;
		go_ack_count = 0;
		for(i = 0 ; i < count ; i++){
			addr[i] = (struct host_addr *)malloc(sizeof(struct host_addr));
			addr[i] -> IP = ip[i];
			addr[i] -> port = ports[i];
			addr[i] -> read_flag = read_flag;
			strcpy(addr[i] -> rqst,  request);
			if((ret = pthread_create(&thread_array[i], NULL, two_pc_protocol, (void *)addr[i])) == -1){
				printf("Thread creation failed with return code: %d\n", ret);
				exit(ret);
			}
			if(read_flag)break; // Query once for the read request.
		}
		for(i = 0 ; i < count ; i++){
			pthread_join(thread_array[i], (void **) &(status[i]));
			if(read_flag)break; // Query once for the read request
		}
//    pthread_exit(0);
		return true;
}
