#C Compiler to use.
CC = gcc

# C Flags to use.
CFLAGS = -c

all: accessqueue.exec clean

accessqueue.exec: accessqueue.o keyval.o
	$(CC) accessqueue.o keyval.o -o accessqueue.exec

accessqueue.o: accessqueue.c
	$(CC) $(CFLAGS) accessqueue.c -o accessqueue.o

keyval.o: keyval.c
	$(CC) $(CFLAGS) keyval.c -o keyval.o

clean:
	rm *.o
