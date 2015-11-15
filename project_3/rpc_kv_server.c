/*
	Name : Jayant Gupta
	Date : November 13, 2015

*/

#include "rpc_kv.h"
#include "keyval.h"
#include <unistd.h>
#include <pthread.h>

static pthread_mutex_t lock;

int readers = 0;
bool readLock = false;
bool writeLock = false;

bool_t
put_1_svc(row *argp, bool_t *result, struct svc_req *rqstp)
{
	char request[1024]="";
	pthread_mutex_lock(&lock);
	printf("Lock Acquired\n");
	printf("Inside PUT Request\n");
	//*result = _PUT(argp->key, argp->value);
	strcat(request, "PUT:"); 	strcat(request, argp->key);
	strcat(request, ":"); strcat(request, argp->value);
	*result = run(request);
	printf("Lock Released\n");
	pthread_mutex_unlock(&lock);
	return(TRUE);
}

bool_t
get_1_svc(int *argp, char **result, struct svc_req *rqstp)
{
	pthread_mutex_lock(&lock);
	printf("Lock Acquired\n");
	printf("Inside GET Request\n");
	*result = (char *)malloc(sizeof(char)*26);
	strcpy(*result, _GET(*argp));
	printf("Lock Released\n");
	pthread_mutex_unlock(&lock);
	return(TRUE);
}

bool_t
delete_1_svc(int *argp, bool_t *result, struct svc_req *rqstp)
{
	char request[1024]="";
	char key[10];
	sprintf(key, "%d", *argp);
	writeLock = true;  // Acquire Lock */
	pthread_mutex_lock(&lock);
	printf("Lock Acquired\n");
	printf("Inside DELETE request\n");
	strcat(request, "DELETE:"); strcat(request, key);
	*result = run(request);
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
