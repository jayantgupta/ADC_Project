/*
	proposer.c 
	PAXOS proposer
	Authors: Jayant Gupta, Alaka Sukumar
	Date. Dec 7, 2015.
*/
#include "proposer.h"

int promise_count;
int accepted_count;
pthread_mutex_t thread_lock;
const char *SYSTEM_FILE = "SYSTEM_buffer";
const char *INIT_PROMISE = "INIT_Promise";
  
/* current highest seq number of proposer */
//long unsigned int current_id = 0; // will it always start from this?

//init method to initialize values on node reboot */
long unsigned int init(){
				/* read and initialize promise id*/
				long unsigned int prom_id;
				FILE *fp = fopen(INIT_PROMISE, "r");
				fscanf(fp, "%lu", &prom_id); // Copied alive nodes from file.
				fclose(fp);

				printf("Promise id initialized is  : %lu \n", prom_id);
				return prom_id;
}

void get_and_add_promise(){
			pthread_mutex_lock(&thread_lock);
			promise_count++;
			pthread_mutex_unlock(&thread_lock);			
}

void get_and_add_accepted(){
			pthread_mutex_lock(&thread_lock);
			accepted_count++;
			pthread_mutex_unlock(&thread_lock);
}

int create_tcp_connection(char *IP, char *port){
				int sockfd;
				printf("Connect to %s %s \n", IP, port);
				struct sockaddr_in server_addr;
				struct hostent *host = gethostbyname(IP);
				if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
								perror("Socket");
								printf("Unable to create socket\n");
								int exit_status  = 1;
								pthread_exit(&exit_status);
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
								int exit_status = 1;
								pthread_exit(&exit_status);
				}
				printf("Connection Done !!!");
				return sockfd;
}

int get_quorum(){
				/* compute quorum by checking number of nodes alive */
				FILE *fs = fopen(SYSTEM_FILE, "r");
				char c[10];
				fscanf(fs, "%s", c); // Copied alive nodes from file.
				fclose(fs);
				int alivenodes = atoi(c);
				return (alivenodes / 2) + 1;
}

char ** get_tokens(char *recv_buff){
				/* split promise msg on ":" */
				const char s[2] = ":";
				char *token;
				/* get the first token */
				token = strtok(recv_buff, s); 
				char **allTokens;
				allTokens = (char **)malloc(sizeof(char *)*3);   
				int i = 0;
				while( token != NULL && i < 3)   
				{ 
								allTokens[i] = (char *)malloc(sizeof(char)*26);
								printf("%s\n", token);	
								strcpy(allTokens[i], token);
								token = strtok(NULL, s);
								i++;
				}
				return allTokens;
}

char *prepare_message(long unsigned int current_id,char *request){
				char str[15];
				sprintf(str, "%lu", current_id);
			  strcat(str, ":");	
				printf("The current id string is : %s\n", str);
				char *preparemessage = (char *)malloc(sizeof(char) * 50);
				strcpy(preparemessage,  "prepare:");
				strcat(preparemessage, str);
				strcat(preparemessage, request);// This is the prepare message
				return preparemessage;
}


char *accept_message(char *promise_id, char *request){
				char *acceptmessage = (char *)malloc(sizeof(char)*50);
				strcpy(acceptmessage,  "accept:");
				strcat(acceptmessage, promise_id);// accept with promise id received from acceptors.
				strcat(acceptmessage, ":");
				strcat(acceptmessage, request); // this is accept message
				return acceptmessage;
}

// Executes the propose protocol of Paxos
void *propose(void *addr){
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

				int sockfd, recv_bytes;
				char send_buff[1024], recv_buff[1024];

				/*Initialize tcp connection */	
				sockfd = create_tcp_connection(IP, port);
				int quorum = get_quorum();
				
				/* send Prepare to the connected server */
				long unsigned int proposal_id = init() + 1; //current_id;
				char *preparemessage = prepare_message(proposal_id,request);

				printf("prepare msg is %s \n", preparemessage);
				send(sockfd, preparemessage, strlen(preparemessage), 0); // prepare
				/* receive promise from quorum */
				recv_bytes = recv(sockfd, recv_buff, 1024, 0); // prepare response
				printf("%s:%s:%s \n", IP, port, recv_buff);
				char ** allTokens = get_tokens(recv_buff);
				char *receivedmsg = allTokens[0];
				char *promise_id = allTokens[1];
				
				/* update proposer's current id to received promise id */
				long unsigned int received_prom_id = atol(promise_id);
				long unsigned int current_id = received_prom_id; // change the current_id to received prom id. ??

				if(strcmp(receivedmsg, "promise" ) == 0){
								get_and_add_promise();
				}
				else if(strcmp(receivedmsg, "NACK" ) == 0){
								/* if NACK received from acceptor */
								current_id = current_id + 1; // increment current id and resend
								preparemessage = prepare_message(current_id,request);
								printf("prepare msg is %s \n", preparemessage);
								send(sockfd, preparemessage, strlen(preparemessage), 0); // resend prepare  

				}
				printf("promise count : %d\n", promise_count);

				while(promise_count < quorum && !read_flag){
								//spin;TODO add timeout here.
				}

				/* Send Accept */
				char *acceptmessage = accept_message(promise_id, request);
				printf("accept msg is %s \n", acceptmessage);
				send(sockfd, acceptmessage, strlen(acceptmessage), 0);

				/* receive accepted by quorum */
				recv_bytes = recv(sockfd, recv_buff, 1024, 0);
				printf("%s:%s:%s \n", IP, port, recv_buff);
				char **allAcceptedTokens = get_tokens(recv_buff);            
				char *receivedacceptedmsg = allAcceptedTokens[0];
				char *accepted_id = allAcceptedTokens[1];
				/* update proposer's current id to received accepted id */
				long unsigned int received_acc_id = atol(allAcceptedTokens[1]);
				current_id = received_acc_id; // Changing current_id to received accepted_id. ??

				if(strcmp(receivedacceptedmsg, "accepted" ) == 0){
								get_and_add_accepted();
				}
				else if(strcmp(receivedacceptedmsg, "NACK" ) == 0){
								/* if NACK received from acceptor */
								current_id = current_id + 1; // increment current id and resend
								preparemessage = prepare_message(current_id,request);
								printf("prepare msg is %s \n", preparemessage);
								send(sockfd, preparemessage, strlen(preparemessage), 0); // resend prepare  
				}
				printf("accepted count : %d\n", accepted_count);
				
				/* check if accepted is received by quorum */
				while(accepted_count < quorum && !read_flag){
								//spin; 
				}
				printf("%s:%s:%d\n", IP, port, accepted_count);

				/* Announce for learner*/
				send(sockfd, request, strlen(request), 0);
				close(sockfd);
				pthread_exit(0);
				return NULL ;
}

// Proposer.
// This will be called by rpc_kev_server
bool run(char request[1024], bool read_flag){
				printf("%s\n", request);
				pthread_attr_t attr;
				pthread_attr_init(&attr);
				pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
				printf("%s\n", request);
				int i, ret; // Count of Peers.
				char *ip[] = {"localhost", "localhost"}; // add ip here.
				char *ports[] = {"10000", "10001"};  // add ports here.
				int count = sizeof(ip) / sizeof(ip[0]);
				struct host_addr
				{
								char *IP;
								char *port;
								char rqst[1024];
								bool read_flag;
				};
				struct host_addr **addr = (struct host_addr **)malloc(sizeof(struct host_addr*)*count);
				pthread_t thread_array[count];
				promise_count = 0;
				accepted_count = 0;
				for(i = 0 ; i < count ; i++){
								addr[i] = (struct host_addr *)malloc(sizeof(struct host_addr));
								addr[i] -> IP = ip[i];
								addr[i] -> port = ports[i];
								addr[i] -> read_flag = read_flag;
								strcpy(addr[i] -> rqst,  request);
								if((ret = pthread_create(&thread_array[i], NULL, propose, (void *)addr[i])) == -1){
												printf("Thread creation failed with return code: %d", ret);
												exit(ret);
								}
								if(read_flag)break; // Query once for the read request.
				}
				for(i = 0 ; i < count ; i++){
								pthread_join(thread_array[i], NULL);
								if(read_flag)break; // Query once for the read request
				}
				//    pthread_exit(0);
				return true;
}
