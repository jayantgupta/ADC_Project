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
#include "proposer.h"

int promise_count;
int accepted_count;
pthread_mutex_t thread_lock;
int count = 2; // Number of nodes.
int quorum;
  
/* current highest seq number of proposer */
long unsigned int current_id = 0;


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
        exit(1);
    }
      
		/* Create Time out values */
                printf("Connection Done !!!");
		tv.tv_sec = 0 ;
		tv.tv_usec = 10000;
		FD_ZERO(&myset);
		FD_SET(sockfd, &myset);
               
		/* Get message to send to the server */
		int send_count = 1;
		int status ;
		/* send prepare msg to all the servers */
                long unsigned int proposal_id = current_id;
                char str[15];
                sprintf(str, "%lu", current_id); 
                printf("The current id string is : %s\n", str);
                char preparemessage[50];
                strcpy(preparemessage,  "prepare:");
                strcat(preparemessage, str);// This is the prepare message
                printf("prepare msg is %s \n",preparemessage);
		send(sockfd, preparemessage, strlen(preparemessage), 0);
		/* receive promise from quorum */
		recv_bytes = recv(sockfd, recv_buff, 1024, 0);
		printf("%s--%s-- \n", port, recv_buff);
                /* split promise msg on ":" */
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
                char *receivedmsg = allTokens[0];
                char *promise_id = allTokens[1];
                /* update proposer's current id to received promise id */
                long unsigned int received_prom_id = atol(allTokens[1]);
                current_id = received_prom_id;
                
		if(strcmp(receivedmsg,"promise" ) == 0){
						get_and_add_promise();
						
		}
		printf("promise count : %d\n", promise_count);
               /* check if promise is received by quorum */
		while(promise_count < quorum && !read_flag){
			//spin;
		}
		printf("%s : %d\n", port, promise_count);
                 /* Send Accept */
                char acceptmessage[50];
                strcpy(acceptmessage,  "accept:");
                strcat(acceptmessage, promise_id);// accept with promise id received from acceptors.
                strcat(acceptmessage, ":");
                strcat(acceptmessage, request); // this is accept message
                                              
                printf("accept msg is %s \n",acceptmessage);
		send(sockfd, acceptmessage, strlen(acceptmessage), 0);

		/* receive accepted by quorum */
		recv_bytes = recv(sockfd, recv_buff, 1024, 0);
		printf("%s--%s-- \n", port, recv_buff);
                /* split accepted msg on ":" */
                const char a[2] = ":";
		char *acceptedstrings;
		/* get the first token */
		acceptedstrings = strtok(recv_buff, a); 
                char allAcceptedTokens[3][26];             
		int j = 0;
		while( acceptedstrings != NULL && j < 3)   
		{ 
					 printf("%s\n", acceptedstrings);	
						strcpy(allAcceptedTokens[j],acceptedstrings);
						acceptedstrings = strtok(NULL, a);
						j++;
		}
                char *receivedacceptedmsg = allAcceptedTokens[0];
                char *accepted_id = allAcceptedTokens[1];
                /* update proposer's current id to received accepted id */
               long unsigned int received_acc_id = atol(allAcceptedTokens[1]);
               current_id = received_acc_id;
                
		if(strcmp(receivedacceptedmsg,"accepted" ) == 0){
						get_and_add_accepted();
						
		}
		printf("accepted count : %d\n", accepted_count);
               /* check if accepted is received by quorum */
		while(accepted_count < quorum && !read_flag){
			//spin;
		}
		printf("%s : %d\n", port, accepted_count);
                
                /* Announce */
                send(sockfd, request, strlen(request), 0);

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
		char *ip[] = {"localhost", "localhost"}; // add ip here.
		char *ports[] = {"10000", "10001"};  // add ports here.
//		char *ip[] = {"localhost", "localhost"}; // add ip here.
//		char *ports[] = {"10000", "10001"};  // add ports here.
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
                quorum = (count/2) + 1 ; 
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
			pthread_join(thread_array[i], (void **) &(status[i]));
			if(read_flag)break; // Query once for the read request
		}
//    pthread_exit(0);
		return true;
}
