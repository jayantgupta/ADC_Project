INSTALLATION.

Assuming the system is to installed on n01-n09 on the server.


To build the executables run the following command.
	make -f Makefile.rpc_kv

To run the program.

Server side. Run the following commands on the all the nodes.

~/project_3/rpc_kv_server
~/project_3/2pc_server 10000

Client side

<Code-Directory>./rpc_kv_client <server-IP> 
(Server-IP can be any value from n01 to n09)

After the client connects to the server, GET, PUT and DELETE requests can be sent to the server.

Added.

heartbeat.c # A tcp server that is used to give heartbeat signals.
selector.c # Selector to read the heartbeat from each server and chose the leader (if applicable).
