/* acceptor.c 
	 Paxos Acceptor
	 Authors: Jayant Gupta, Alaka Sukumar
	 Updated Date : November 13, 2015
 */

#include "keyval.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

const char *GET_FILE = "GET_buffer";
long unsigned int promised_id;// greatest promised id
long unsigned int current_accepted_id;// greatest accepted id seen by acceptor
const char *INIT_Promise = "INIT_Promise"; // store greatest promised id in this file
const char *INIT_Accepted = "INIT_Accepted"; // store greatest accepted id in this file

void init(long unsigned int prom_id, long unsigned int accep_id);
int create_tcp_server(char *port);
char **get_tokens(char *recv_buff);
void store_buffer(const char *buffer_file, long unsigned int ID);
char *learner(char *);
char *promise_message(long unsigned int promised_id);
char *accepted_message(long unsigned int current_accepted_id);

int main(int argc, char *argv[]){
				if(argc != 2){
								printf("Error\nUsage ./2pc_server.exec port\n");
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
				//sync(current_accepted_id); // To start syncing.
				sockfd = create_tcp_server(argv[1]);

				/* server listening */
				printf("Server Listening ..\n");
				fflush(stdout);
				while(1){
								char send_buff [1024] , recv_buff[1024];
								/* accept the connection */
								newsock = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
								printf("Accepted Request\n");
								// Serve first request.Check if sequence number less than its own stored seq number.
								/* Phase 1 - receive proposal */ 

								recv_bytes = recv(newsock, recv_buff, 1024, 0);
								recv_buff[recv_bytes] = '\0';
								printf("%s\n", recv_buff);
								char ** allTokens = get_tokens(recv_buff);             
								printf("The receieve proposer id is : %s \n", allTokens[1]);
							
								long unsigned int received_prop_id = atol(allTokens[1]);

								/* check if current received proposal id is greater than promised_id*/
								if (received_prop_id >= promised_id){                        
												promised_id = received_prop_id; 
												/* store promised id in INIT_Promise file */
												store_buffer(INIT_Promise, promised_id);
								}else{
									//abort and re-initiate.
								}
												
								/* send promise message */
								char *promisemessage = promise_message(promised_id);
								printf("Promise Message is : %s\n", promisemessage);
								send(newsock, promisemessage, strlen(promisemessage), 0);// Send back the promise.

								/*Phase 2 - Receive Accept  */
								recv_bytes = recv(newsock, recv_buff, 1024, 0);
								recv_buff[recv_bytes] = '\0'; 
								/* process Accept msg */
								char **allAcceptTokens = get_tokens(recv_buff);             
								char *accept_id = allAcceptTokens[1];
								char *request = allAcceptTokens[2];
								long unsigned int received_accept_id = atol(allAcceptTokens[1]);

								/* check if promised id is still greater than or equal to accept id received from proposer */
								if (received_accept_id >= promised_id){
												current_accepted_id = received_accept_id; 
												/* store accepted id in INIT_Accepted file */
												store_buffer(INIT_Accepted, current_accepted_id);
												
												/* send accepted message to proposer */
												char *acceptedmessage = accepted_message(current_accepted_id);
												printf("Accepted Message is : %s\n", acceptedmessage); 
												send(newsock, acceptedmessage, strlen(acceptedmessage), 0);
								}
								else{ 
									// abort and re-initiate 								
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

char *promise_message(long unsigned int promised_id){
				char str[15]; 
				sprintf(str, "%lu", promised_id);              
				char *promisemessage = (char *)malloc(sizeof(char)*50);
				strcpy(promisemessage,  "promise:");
				strcat(promisemessage, str);// This is the promise message
				return promisemessage;

}

char *accepted_message(long unsigned int current_accepted_id){
				char accepted_id_for_msg[15];
				sprintf(accepted_id_for_msg, "%lu", current_accepted_id); 
				char *acceptedmessage = (char *)malloc(sizeof(char)*50);
				strcpy(acceptedmessage,  "accepted:");
				strcat(acceptedmessage, accepted_id_for_msg);// This is the accepted message
				return acceptedmessage;
}


// Takes the first request and responds as ACK.
// Assuming all the errors are checked at the coordinator-server
char * learner(char * request){
		printf("Executing Request : %s\n", request);
		/* code to validate the request*/
		const char s[2] = ":";
		char *token;
		/* get the first token */
		token = strtok(request, s);            
		char allTokens[3][26];             
		int i = 0;
		while( token != NULL && i < 3)   
		{ 
					 printf("%s\n", token);	
						strcpy(allTokens[i],token);
						token = strtok(NULL, s);
						i++;
		}
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
