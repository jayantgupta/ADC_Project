#C Compiler to use.
CC = gcc

#C Flags to use.
CFLAGS = -c

all: tcpserver.exec tcpclient.exec udpserver.exec udpclient.exec clean

tcpserver.exec: tcpserver.o keyval.o logfile.o
	$(CC) tcpserver.o keyval.o logfile.o -o tcpserver.exec

tcpclient.exec: tcpclient.o logfile.o
	$(CC) tcpclient.o logfile.o -o tcpclient.exec

udpserver.exec: udpserver.o keyval.o logfile.o
	$(CC) udpserver.o keyval.o logfile.o -o udpserver.exec

udpclient.exec: udpclient.o logfile.o
	$(CC) udpclient.o logfile.o -o udpclient.exec

tcpclient.o: tcpclient.c
	$(CC) $(CFLAGS) tcpclient.c -o tcpclient.o

tcpserver.o: tcpserver.c
	$(CC) $(CFLAGS) tcpserver.c -o tcpserver.o

udpserver.o: udpserver.c
	$(CC) $(CFLAGS) udpserver.c -o udpserver.o

udpclient.o:udpclient.c
	$(CC) $(CFLAGS) udpclient.c -o udpclient.o

keyval.o: keyval.c
	$(CC) $(CFLAGS) keyval.c -o keyval.o

logfile.o: logfile.c
	$(CC) $(CFLAGS) logfile.c -o logfile.o

clean:
	rm *.o
