/* acceptor.c 
	 Paxos Acceptor
	 Authors: Jayant Gupta, Alaka Sukumar
	 Updated Date : November 13, 2015
 */

#include "keyval.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

const char *GET_FILE = "GET_buffer";
const char *INIT_Promise = "INIT_Promise"; // store greatest promised id in this file
const char *INIT_Accepted = "INIT_Accepted"; // store greatest accepted id in this file
long unsigned int promised_id;// greatest promised id
long unsigned int current_accepted_id;// greatest accepted id seen by acceptor
char *current_accepted_request = "";
char sync_log[20000][60]; // stores the requests learnt till now.

int create_tcp_connection(char *IP, char *port);
void init(long unsigned int prom_id, long unsigned int accep_id);
int create_tcp_server(char *port);
char **get_tokens(char *recv_buff);
void store_buffer(const char *buffer_file, long unsigned int ID);
char *learner(char *);
char *build_message(char *msg, long unsigned int promised_id);
void server_sync(long unsigned int id);
void serve_sync_request(int sockfd, long unsigned int current_id);
void sleep_me();

int main(int argc, char *argv[]){
				if(argc != 2){
								printf("Error\nUsage ./acceptor.exec port\n");
								exit(1);
				}	
				/*
					 Setting up the key-value directory.
				 */
				int sockfd, newsock, recv_bytes ;
				struct sockaddr_in client_addr;
				int sin_size =  sizeof(struct sockaddr_in);
				const char *log_filename = "tcp_server.log";

				/* call init function to initialize the values */
				init(promised_id, current_accepted_id);
				/*
					 send sync request to the known servers.
					 They respond with their logs, if their ID is greater than this server's id.
					 Otherwise, they abandon the task.

					 When they receive the sync request does the same thing as described above but 
					 instead sends its own command history.
				 */
				sockfd = create_tcp_server(argv[1]);

				/* server listening */
				printf("Acceptor Listening ..\n");
				fflush(stdout);
				while(1){
								sleep_me();
								char send_buff [1024] , recv_buff[1024];
								/* accept the connection */
								newsock = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
								printf("Accepted Request\n");
								// Serve first request
								// Check if sequence number less than its own stored seq number
								/* Phase 1 - receive proposal */ 
				PREPARE:
								recv_bytes = recv(newsock, recv_buff, 1024, 0);
								recv_buff[recv_bytes] = '\0';
								printf("Received msg: %s\n", recv_buff);
								char ** allTokens = get_tokens(recv_buff);             
								if(strcmp(allTokens[0], "SYNC") == 0){
												serve_sync_request(newsock, atoi(allTokens[1]));
												continue;
								}
								printf("The received proposer id is : %s \n", allTokens[1]);
								printf("The received request is : %s \n", allTokens[2]);
								printf("Current promised id is : %lu \n", promised_id);
								current_accepted_request = allTokens[2];

								long unsigned int received_prop_id = atol(allTokens[1]);

								/* check if current received proposal id is greater than promised_id*/
								if (received_prop_id >= promised_id ){                        
												promised_id = received_prop_id; 
												/* store promised id in INIT_Promise file */
												printf("Storing promise buffer\n");
												store_buffer(INIT_Promise, promised_id);
								}else{
												/* send NACK */
												char *nackmessage = build_message("NACK:", received_prop_id);
												printf("NACK Message is : %s\n", nackmessage);
												send(newsock, nackmessage, strlen(nackmessage), 0);// Send back NACK 
												goto PREPARE;
								}

								/* send promise message */
								char *promisemessage = build_message("promise:", promised_id);
								printf("Promise Message is : %s\n", promisemessage);
								send(newsock, promisemessage, strlen(promisemessage), 0);// Send back the promise.

								/*Phase 2 - Receive Accept  */
								recv_bytes = recv(newsock, recv_buff, 1024, 0);
								recv_buff[recv_bytes] = '\0'; 
								/* process Accept msg */
	 							printf("Accepted Request : %s\n", recv_buff);							
								char **allAcceptTokens = get_tokens(recv_buff);
								char *accept_id = allAcceptTokens[1];
								char *request = allAcceptTokens[2];
								long unsigned int received_accept_id = atol(allAcceptTokens[1]);

								/* check if promised id is still greater than or equal to accept id received from proposer and if request promised is same as accepted request*/
								if (received_accept_id >= promised_id && strcmp(current_accepted_request, request) == 0 ){
												current_accepted_id = received_accept_id; 
												/* store accepted id in INIT_Accepted file */
												printf("Storing Accepted buffer\n");
												store_buffer(INIT_Accepted, current_accepted_id);
												/* add values to sync log*/
												strcpy(sync_log[current_accepted_id], accept_id);
												strcat(sync_log[current_accepted_id], ":");
												strcat(sync_log[current_accepted_id], request);

												/* send accepted message to proposer */
												char *acceptedmessage = build_message("accepted:", current_accepted_id);
												printf("Accepted Message is : %s\n", acceptedmessage); 
												send(newsock, acceptedmessage, strlen(acceptedmessage), 0);
								}
								else{ 
												/* send NACK */
												char *nackacceptmessage = build_message("NACK:", received_accept_id);
												printf("NACK Message is : %s\n", nackacceptmessage);
												send(newsock, nackacceptmessage, strlen(nackacceptmessage), 0);// Send back NACK 
												goto PREPARE;								
								}

								/* invoke learner */
								recv_bytes = recv(newsock, recv_buff, 1024, 0);
								recv_buff[recv_bytes] = '\0'; 
								learner(recv_buff); // learner.
								printf("Request : %s\n", request);

								close(newsock);
				}
				close(sockfd);
				return 0;
}

void sleep_me(){
	int p;
	p = rand();
	srand((unsigned)time(NULL));
	int r = rand();
	if((p + r) % 5 == 0){
		printf("\n----\nSending myself to sleep for 10 seconds\n----\n");
		sleep(10);
	}
	return;
}

char * build_message(char *msg, long unsigned int ID){
				char str[15];
				sprintf(str, "%lu", ID);
				char *message = (char *)malloc(sizeof(char) * 50);
				strcpy(message, msg);
				strcat(message, str);
				return message;
}

void serve_sync_request(int newsock, long unsigned int sync_accepted_id){
				long unsigned int current_accepted_id;
				int recv_bytes, i;
				char recv_buff[1024];
				char **allTokens;
				if(sync_accepted_id >= current_accepted_id){
								send(newsock, "NO_DATA", strlen("NO_DATA"), 0);
				}	
				else{
								send(newsock, "DATA", strlen("DATA"), 0);
								recv_bytes = recv(newsock, recv_buff, 1024, 0);
								allTokens = get_tokens(recv_buff);
								if(strcmp(allTokens[0], "READY") == 0){
												// send all the commands for sync-up.
												for(i = 0 ; i< current_accepted_id ; i++){
																send(newsock, sync_log[i], strlen(sync_log[i]), 0);
												}
												char *donemessage = build_message("DONE:", current_accepted_id);
												send(newsock, donemessage, strlen(donemessage), 0);
								}
				}
				close(newsock);
				return;
}

void server_sync(long unsigned int ID){
				char *ip[] = {"localhost", "localhost"};
				char *ports[] = {"10000", "10001"};
				int count = sizeof(ip) / sizeof(ip[0]);
				int i, recv_bytes;
				char recv_buff[1024];
				for(i = 0 ; i < count ; i++){
								int sockfd = create_tcp_connection(ip[i], ports[i]);
								char *syncmessage = build_message("SYNC:", ID);
								send(sockfd, syncmessage, strlen(syncmessage), 0); // syncing.
								recv_bytes = recv(sockfd, recv_buff, 1024, 0);
								char **allTokens = get_tokens(recv_buff);
								if(strcmp(allTokens[0], "NO_DATA") == 0){
												continue;
								}
								else if(strcmp(allTokens[0], "DATA") == 0){
												send(sockfd, "READY", strlen("READY"), 0);
												char *id, *request;
												do{
																recv_bytes = recv(sockfd, recv_buff, 1024, 0);
																allTokens = get_tokens(recv_buff);
																id = allTokens[0];
																request = allTokens[1]; //extracting the request here.
																if(strcmp(request, "DONE") != 0){
																				learner(request);
																}
												}while(strcmp(request, "DONE") == 0);
								}
				}
}

// Establishes a tcp connection with a tcp server on the given IP and port.
int create_tcp_connection(char *IP, char *port){
				int sockfd;
				printf("Connect to %s %s \n", IP, port);
				struct hostent *host = (struct hostent *)gethostbyname(IP);
				struct sockaddr_in server_addr;
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

//init method to initialize values on node reboot */
void init(long unsigned int prom_id, long unsigned int accep_id){
				/* read and initialize promise id*/

				FILE *fp = fopen(INIT_Promise, "r");
				fscanf(fp, "%lu", &prom_id); // Copied alive nodes from file.
				fclose(fp);

				printf("Promise id initialized is  : %lu \n",prom_id);

				/* read and initialize accepted id*/

				FILE *fs = fopen(INIT_Accepted, "r");
				fscanf(fs, "%lu", &accep_id); // Copied alive nodes from file.
				fclose(fs);

				printf("Accepted id initialized is  : %lu \n",accep_id);

				printf("Initialized\n"); 
}

int create_tcp_server(char *port){
				int sockfd;
				bool val = true;
				struct sockaddr_in server_addr ;/* creating a socket */
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
				server_addr.sin_port = htons(atoi(port));
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
				return sockfd;
}

char **get_tokens(char *recv_buff){
				/* split prepare msg on ":" */
				const char s[2] = ":";
				char *token;
				/* get the first token */
				token = strtok(recv_buff, s); 
				char **allTokens = (char **)malloc(sizeof(char *)*3);             
				int i = 0;
				while( token != NULL && i < 3)   
				{ 
								allTokens[i] = (char *)malloc(sizeof(char)*26);
								///								printf("%s\n", token);	
								strcpy(allTokens[i], token);
								token = strtok(NULL, s);
								i++;
				}
				return allTokens;

}

void store_buffer(const char *buffer_file, long unsigned int ID){
				FILE *fp = fopen(buffer_file, "w");
				fprintf(fp, "%lu", ID);
				fclose(fp);
}

// Takes the first request and responds as ACK.
// Assuming all the errors are checked at the coordinator-server
char * learner(char * request){
				printf("Executing Request : %s\n", request);
				/* code to validate the request*/
				/*		const char s[2] = ":";
							char *token;
				//		get the first token 
				token = strtok(request, s);            
				char allTokens[3][26];             
				int i = 0;
				while( token != NULL && i < 3)   
				{ 
				printf("%s\n", token);	
				strcpy(allTokens[i],token);
				token = strtok(NULL, s);
				i++;
				}*/
				char ** allTokens = get_tokens(request);
				if(strcmp(allTokens[0],"GET") == 0){ 
								printf("Called method GET\n");
								int key = atoi(allTokens[1]);
								char *getValue = _GET(key);
								FILE *fp = fopen(GET_FILE, "w");
								fputs(getValue, fp);
								fclose(fp);
				} 
				else if(strcmp(allTokens[0],"PUT") == 0){
								printf("Inside PUT %s %s\n", allTokens[1], allTokens[2]);
								bool putValue = _PUT(atoi(allTokens[1]), allTokens[2]); 
								if(putValue){
												return "COMMIT";
								}
								else{
												return "PUT_ERROR";
								}

				}
				else if(strcmp(allTokens[0],"DELETE") == 0){
								printf("Inside DELETE\n");
								bool delValue = _DELETE(atoi(allTokens[1]));
								if(delValue){
												return "COMMIT";
								}
								else{
												return "DELETE_ERROR";
								}                
				}
				return "UN_KNOWN_REQUEST";
}
