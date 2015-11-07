NOTE:- 

Changed the function name in keyval.h as follows
PUT -> _PUT
GET-> _GET
DELETE->_DELETE

This is done to avoid conflict with the GET, PUT and DELETE definitions in 
rpc_kv.x file


INSTALLATION.


To build the executables run the following command.
	make -f Makefile.rpc_kv

To run the program.

Server side

<Code-Directory>./rpc_kv_server

Client side

<Code-Directory>./rpc_kv_client <server-IP>

After the client connects to the server, GET, PUT and DELETE requests can be sent to the server.
