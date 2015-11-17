#ifndef COORDINATOR_H_INCLUDED 
#define COORDINATOR_H_INCLUDED

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/select.h>
#include <pthread.h>

void get_and_add_ack();
void get_and_add_go_ack();
void *two_pc_protocol(void *addr);
bool run(char request[1024]);
#endif
