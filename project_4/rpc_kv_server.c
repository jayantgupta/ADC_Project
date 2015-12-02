/*
	Name : Jayant Gupta
	Date : November 13, 2015
*/

#include "rpc_kv.h"
#include "keyval.h"
#include "coordinator.h"
#include <unistd.h>
#include <pthread.h>

static pthread_mutex_t lock;
const char *GET_FILE = "GET_buffer";

bool_t
put_1_svc(row *argp, bool_t *result, struct svc_req *rqstp)
{
	char request[1024]="";
	char key[20];
	sprintf(key, "%d", argp->key);
	pthread_mutex_lock(&lock);
	printf("Lock Acquired\n");
	printf("Inside PUT request\n");
	strcat(request, "PUT:"); 
	strcat(request, key);
	strcat(request, ":");
	strcat(request, argp->value);
	*result = run(request, false);
	printf("Lock Released\n");
	pthread_mutex_unlock(&lock);
	return(TRUE);
}

bool_t
get_1_svc(int *argp, char **result, struct svc_req *rqstp)
{
	char request[1024]="";
	char key[20];
	pthread_mutex_lock(&lock);
	printf("Lock Acquired\n");
	printf("Inside GET Request\n");
	strcat(request, "GET:");
	sprintf(key, "%d", *argp);
	strcat(request, key);
	run(request, true); // read flag = true.
	*result = (char *)malloc(sizeof(char)*26);
	FILE *fp = fopen(GET_FILE, "r");
	fscanf(fp, "%s", *result); // Copied result from file.
	fclose(fp);
	printf("%s\n", *result);
//	strcpy(*result, _GET(*argp));
	printf("Lock Released\n");
	pthread_mutex_unlock(&lock);
	return(TRUE);
}

bool_t
delete_1_svc(int *argp, bool_t *result, struct svc_req *rqstp)
{
	char request[1024]="";
	char key[20];
//	sprintf(key, "%d", *argp);
//	writeLock = true;  // Acquire Lock */
	pthread_mutex_lock(&lock);
	printf("Lock Acquired\n");
	printf("Inside DELETE request\n");
	strcat(request, "DELETE:"); 
	sprintf(key, "%d", *argp);
	strcat(request, key);
	*result = run(request, false);
	printf("Lock Released\n");
	pthread_mutex_unlock(&lock);
	return(TRUE);
}

int
keyval_prog_1_freeresult (SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result)
{
	xdr_free (xdr_result, result);
	return 1;
}
