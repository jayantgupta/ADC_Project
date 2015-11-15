LINK.c = gcc

all: 2pc_server coordinator

2pc_server : 2pc_server.o keyval.o
	$(LINK.c) -o 2pc_server 2pc_server.o keyval.o 

keyval.o : keyval.c
		$(LINK.c) -c keyval.c

2pc_server.o : 2pc_server.c
		$(LINK.c) -c 2pc_server.c

coordinator : coordinator.c
		$(LINK.c) -o coordinator coordinator.c -lpthread

my_clean:
	   rm *.o

