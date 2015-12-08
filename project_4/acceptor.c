/* tcpserver.c 
	 Serving 2-phase commit requests amongst various servers
	 Initial Version : Alaka Sukumar
	 2-phase commit version : Jayant Gupta
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
char *exec_rqst(char *);
long unsigned int promised_id;// greatest promised id
long unsigned int current_accepted_id;// greatest accepted id seen by acceptor
char *request; // the request to be processed
const char *INIT_Promise = "INIT_Promise"; // store greatest promised id in this file
const char *INIT_Accepted = "INIT_Accepted"; // store greatest accepted id in this file

//init method to initialize values on node reboot */
void init(long unsigned int prom_id,long unsigned int accep_id){
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

int main(int argc, char *argv[]){
	if(argc != 2){
		printf("Error\nUsage ./2pc_server.exec port\n");
		exit(1);
	}	
	/*
		Setting up the key-value directory.
	*/
	int sockfd, newsock, recv_bytes ;
	bool val = true;
	struct sockaddr_in server_addr, client_addr;
	int sin_size;
        const char *log_filename = "tcp_server.log";

  /* call init function to initialize the values */
  init(promised_id, current_accepted_id);
	sync(current_accepted_id); // 
	
	/* creating a socket */
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
	server_addr.sin_port = htons(atoi(argv[1]));
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

	/* logging server listening */
  printf("Server Listening ..\n");
	fflush(stdout);
	while(1){
		char send_buff [1024] , recv_buff[1024];
		sin_size = sizeof(struct sockaddr_in);
                 int send_count = 1;
                 int status ;
		/* accept the connection */
		newsock = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
		printf("Accepted Request\n");
		// Serve first request.Check if sequence number less than its own stored seq number.
    /* Phase 1 - receive proposal */ 
                
		recv_bytes = recv(newsock, recv_buff, 1024, 0);
		recv_buff[recv_bytes] = '\0';
		printf("%s\n", recv_buff);
                /* split prepare msg on ":" */
                const char s[2] = ":";
		char *token;
		/* get the first token */
		token = strtok(recv_buff, s); 
                char allTokens[3][26];             
		int i = 0;
		while( token != NULL && i < 3)   
		{ 
					 printf("%s\n", token);	
						strcpy(allTokens[i],token);
						token = strtok(NULL, s);
						i++;
		}
                 printf("The receieve proposer id is : %s \n", allTokens[1]);
                 long unsigned int received_prop_id = atol(allTokens[1]);
                 
                 /* check if current promised id is equal to or greater than received proposal id */
                 if (promised_id == received_prop_id || received_prop_id > promised_id){                        
                 promised_id= received_prop_id; 
                 /* store promised id in INIT_Promise file */
                 FILE *fp1 = fopen(INIT_Promise, "w");
						fprintf(fp1, "%lu", promised_id);
						fclose(fp1);


                /* send promise message */
                
                char str[15]; 
                sprintf(str, "%lu", promised_id);              
                char promisemessage[50];
                strcpy(promisemessage,  "promise:");
                strcat(promisemessage, str);// This is the promise message
                printf("Promise Message is : %s\n", promisemessage);
		send(newsock, promisemessage, strlen(promisemessage), 0);// Send back the promise.
                }

		/*Phase 2 - Receive Accept  */
		recv_bytes = recv(newsock, recv_buff, 1024, 0);
		recv_buff[recv_bytes] = '\0'; 
                /* process Accept msg */
                const char c[2] = ":";
		char *acceptstring;
		/* get the first token */
		acceptstring = strtok(recv_buff, c); 
                char allAcceptTokens[3][26];             
		int j = 0;
		while( acceptstring != NULL && j < 3)   
		{ 
					 printf("%s\n", acceptstring);	
						strcpy(allAcceptTokens[j],acceptstring);
						acceptstring = strtok(NULL, c);
						j++;
		}
                char *accept_id = allAcceptTokens[1];
                request = allAcceptTokens[2];
                long unsigned int received_accept_id = atol(allAcceptTokens[1]);
                
                /* check if promised id is greater than accept id received from proposer */
                if (promised_id == received_accept_id || received_accept_id > promised_id){
                current_accepted_id = received_accept_id; 
                /* store accepted id in INIT_Accepted file */
                 FILE *fp2 = fopen(INIT_Accepted, "w");
						fprintf(fp2, "%lu", current_accepted_id);
						fclose(fp2);
                /* send accepted message to proposer */
                char accepted_id_for_msg[15];
                sprintf(accepted_id_for_msg, "%lu", current_accepted_id); 
                char acceptedmessage[50];
                strcpy(acceptedmessage,  "accepted:");
                
                strcat(acceptedmessage, accepted_id_for_msg);// This is the accepted message
                printf("Accepted Message is : %s\n", acceptedmessage); 
		send(newsock, acceptedmessage, strlen(acceptedmessage), 0);
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
//					callLog("Called method GET \n", log_filename);
						printf("Called method GET\n");
						int key = atoi(allTokens[1]);
						char *getValue = _GET(key);
//					send(newsock, getValue, strlen(getValue), 0);    
						FILE *fp = fopen(GET_FILE, "w");
						fputs(getValue, fp);
						fclose(fp);
		} 
		else if(strcmp(allTokens[0],"PUT") == 0){
//					callLog("Called method PUT \n", log_filename);
						printf("Inside PUT %s %s\n", allTokens[1], allTokens[2]);
						bool putValue = _PUT(atoi(allTokens[1]), allTokens[2]); 
						if(putValue){
//							callLog("Successfully Updated\n", log_filename);
								return "COMMIT";
						}

						else{
//						callLog("Error in updating \n", log_filename);
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
