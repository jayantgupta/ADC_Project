/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <memory.h> /* for memset */
#include "rpc_kv.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 5000000, 0 };

				enum clnt_stat 
put_1(row *argp, bool_t *clnt_res, CLIENT *clnt)
{
				return (clnt_call(clnt, PUT,
																(xdrproc_t) xdr_row, (caddr_t) argp,
																(xdrproc_t) xdr_bool, (caddr_t) clnt_res,
																TIMEOUT));
}

				enum clnt_stat 
get_1(int *argp, char **clnt_res, CLIENT *clnt)
{
				return (clnt_call(clnt, GET,
																(xdrproc_t) xdr_int, (caddr_t) argp,
																(xdrproc_t) xdr_wrapstring, (caddr_t) clnt_res,
																TIMEOUT));
}

				enum clnt_stat 
delete_1(int *argp, bool_t *clnt_res, CLIENT *clnt)
{
				return (clnt_call(clnt, DELETE,
																(xdrproc_t) xdr_int, (caddr_t) argp,
																(xdrproc_t) xdr_bool, (caddr_t) clnt_res,
																TIMEOUT));
}
