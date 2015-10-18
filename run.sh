gcc -c keyval.c -o keyval.o
gcc -c tcpserver.c -o tcpserver.o
gcc -c test.c -o mytest.o
gcc mytest.o keyval.o -o mytest.exec
gcc tcpserver.o keyval.o -o tcpserver.exec
gcc tcpclient.c -o tcpclient.exec
