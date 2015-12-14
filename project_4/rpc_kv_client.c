/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "rpc_kv.h"
#include<stdbool.h>

void init_keyval_store(CLIENT *clnt, char **INIT){
/* initializing the key value store */

				printf("Initializing key-value store\n");
				int j;
				for(j = 0 ; j < 7 ; j++){
								printf("%s\n", INIT[j]);
								bool_t result_1;
								enum clnt_stat retval_1;
								row  put_1_arg;
								char buff[80];
								strcpy (buff, INIT[j]);
								const char *delimiter = ":";
								char *argarray = strtok(buff, delimiter);
								int counttoken = 0;
								char arg[3][25];
								while(argarray != NULL){
												strcpy(arg[counttoken], argarray);
												counttoken += 1;
												argarray = strtok(NULL, delimiter);

								}
								put_1_arg.key = atoi(arg[0]);
								strcpy(put_1_arg.value, arg[1]);
								put_1(&put_1_arg, &result_1, clnt);//call put to initialize key-value store
								if (retval_1 != RPC_SUCCESS) {
												clnt_perror (clnt, "call failed");
								}
								if(result_1 == TRUE){
												printf("PUT SUCCESS\n");
								}else{
												printf("PUT FAILED\n");
								}
								sleep(1);
				}
				printf("key-value store initialized\n");
}

void
keyval_prog_1(char *host)
{
				CLIENT *clnt;
				enum clnt_stat retval_1;
				bool_t result_1;
				row  put_1_arg;
				enum clnt_stat retval_2;
				char *result_2;
				int  get_1_arg;
				enum clnt_stat retval_3;
				bool_t result_3;
				int  delete_1_arg;

#ifndef	DEBUG
					clnt = clnt_create (host, KEYVAL_PROG, KEYVAL_VERS, "udp");
					if (clnt == NULL) {
									clnt_pcreateerror (host);
									exit (1);
					}
#endif	/* DEBUG */
			
			char *INIT[7] = {"0:Monday", "1:Tuesday", "2:Wednesday", "3:Thursday", "4:Friday", "5:Saturday",  "6:Sunday"};
			init_keyval_store(clnt, INIT);
			while(true){
					char cmd[50];
					printf("Enter Command\n-------\nGET:<key>\nPUT:<key>:<value>\nDELETE:<key>\n------\n");
					scanf("%s",cmd);
					printf("-------\n");
					char tokens [3][26];
					char *delim = ":";
					char *token;
					token =strtok(cmd,delim);
					int count = 0;
					while(token != NULL){
						if(count == 3){
										printf("Invalid Input\n");
										break;
						}
//					printf("%s\n",token);
						strcpy(tokens[count],token);
						count++;
						token = strtok(NULL, delim);
					}
					if(count == 3 && token != NULL)break;
					if(strcmp(tokens[0], "PUT")== 0){
									put_1_arg.key = atoi(tokens[1]);
									strcpy(put_1_arg.value, tokens[2]);
									retval_1 = put_1(&put_1_arg, &result_1, clnt);
									if (retval_1 != RPC_SUCCESS) {
													clnt_perror (clnt, "call failed");
									}
									if(result_1 == TRUE){
										printf("PUT SUCCESS\n");
									}else{
										printf("PUT FAILED\n");
									}
					}
					else if(strcmp(tokens[0], "GET")== 0){
									result_2 = (char *)malloc(sizeof(char)*26);
									get_1_arg = atoi(tokens[1]);
									retval_2 = get_1(&get_1_arg, &result_2, clnt);
									if (retval_2 != RPC_SUCCESS) {
													clnt_perror (clnt, "call failed");
									}
									printf("GET:%d => %s\n", get_1_arg, result_2);
					}
					else if(strcmp(tokens[0], "DELETE")== 0){
									delete_1_arg = atoi(tokens[1]);
									retval_3 = delete_1(&delete_1_arg, &result_3, clnt);
									if (retval_3 != RPC_SUCCESS) {
													clnt_perror (clnt, "call failed");
									}
									if(result_3 == TRUE){
										printf("DELETE SUCCESS\n");
									}else{
										printf("DELETE FAILED\n");
									}
					}
					else{
							printf("Unrecognized Command\n");
					}
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */

}

int
main (int argc, char *argv[])
{
	char *host;
	char *client_id;

	if (argc != 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	keyval_prog_1 (host);
exit (0);
}
