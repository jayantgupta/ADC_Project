INSTALLATION.

Assuming the system is to installed on n01-n05 on the server.


To build the executables run the following command.
	make -f Makefile.paxos

To run the program.

Server side. Run the following commands on the all the nodes.

~/project_4/rpc_kv_server
~/project_4/acceptor 10000

Client side

<Code-Directory>./rpc_kv_client <server-IP> 
(Server-IP can be any value from n01 to n05)

After the client connects to the server, GET, PUT and DELETE requests can be sent to the server.

Random sleeps are added at the acceptor side of the code, that depends upon the system time and a random integer generate at each of the nodes. They are used to check the system durability in case, of node failure.
