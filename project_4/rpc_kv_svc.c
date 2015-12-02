/*
 * File generated using rpcgen.
 * Edited by Jayant Gupta to 
 * add support for threads.
 * Date. November 13, 2015
 */

#include "rpc_kv.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

pthread_t p_thread;
pthread_attr_t attr;

pthread_mutex_t lock;
/* Procedure to be run by thread */

void *
serv_request(void *data)
{
	struct thr_data
	{
					struct svc_req *rqstp;
					SVCXPRT *transp;
	}*ptr_data;
	union {
		row put_1_arg;
		int get_1_arg;
		int delete_1_arg;
	} argument;
	union {
		bool_t put_1_res;
		char *get_1_res;
		bool_t delete_1_res;
	} result;
	bool_t retval;
	xdrproc_t _xdr_argument, _xdr_result;
	bool_t (*local)(char *, void *, struct svc_req *);

	ptr_data = (struct thr_data *)data;
	struct svc_req *rqstp = ptr_data->rqstp;
	register SVCXPRT *transp = ptr_data->transp;

	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
		return;

	case PUT:
		_xdr_argument = (xdrproc_t) xdr_row;
		_xdr_result = (xdrproc_t) xdr_bool;
		local = (bool_t (*) (char *, void *,  struct svc_req *))put_1_svc;
		break;

	case GET:
		_xdr_argument = (xdrproc_t) xdr_int;
		_xdr_result = (xdrproc_t) xdr_wrapstring;
		local = (bool_t (*) (char *, void *,  struct svc_req *))get_1_svc;
		break;

	case DELETE:
		_xdr_argument = (xdrproc_t) xdr_int;
		_xdr_result = (xdrproc_t) xdr_bool;
		local = (bool_t (*) (char *, void *,  struct svc_req *))delete_1_svc;
		break;

	default:
		svcerr_noproc (transp);
		return;
	}
	memset ((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		svcerr_decode (transp);
		return;
	}
	retval = (bool_t) (*local)((char *)&argument, (void *)&result, rqstp);
	if (retval > 0 && !svc_sendreply(transp, (xdrproc_t) _xdr_result, (char *)&result)) {
		svcerr_systemerr (transp);
	}
	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		fprintf (stderr, "%s", "unable to free arguments");
		exit (1);
	}
	if (!keyval_prog_1_freeresult (transp, _xdr_result, (caddr_t) &result))
		fprintf (stderr, "%s", "unable to free results");

	return;
}

/*
	New code for procedure keyval_prog_1, starting thread in response for each clients
	request to invoke remote procedure.
 */

static void
keyval_prog_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
	struct data_str
	{
					struct svc_req *rqstp;
					SVCXPRT *transp;
	} *data_ptr = (struct data_str *)malloc(sizeof(struct data_str));
	data_ptr->rqstp = rqstp;
	data_ptr->transp = transp;
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&p_thread, &attr, serv_request, (void *)data_ptr);
}

int
main (int argc, char **argv)
{
	register SVCXPRT *transp;

	pmap_unset (KEYVAL_PROG, KEYVAL_VERS);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, KEYVAL_PROG, KEYVAL_VERS, keyval_prog_1, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (KEYVAL_PROG, KEYVAL_VERS, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, KEYVAL_PROG, KEYVAL_VERS, keyval_prog_1, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (KEYVAL_PROG, KEYVAL_VERS, tcp).");
		exit(1);
	}

	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
	/* NOTREACHED */
}
