#C Compiler to use.
CC = gcc

#C Flags to use.
CFLAGS = -c

all: tcpserver.exec tcpclient.exec

tcpserver.exec: tcpserver.o keyval.o
	$(CC) tcpserver.o keyval.o -o tcpserver.exec

tcpclient.exec: tcpclient.o
	$(CC) tcpclient.o -o tcpclient.exec

tcpclient.o: tcpclient.c
	$(CC) $(CFLAGS) tcpclient.c -o tcpclient.o

tcpserver.o: tcpserver.c
	$(CC) $(CFLAGS) tcpserver.c -o tcpserver.o

keyval.o: keyval.c
	$(CC) $(CFLAGS) keyval.c -o keyval.o

clean:
	rm *.o
