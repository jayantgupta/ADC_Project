#ifndef ACCESSQUEUE_H_INCLUDED
#define ACCESSQUEUE_H_INCLUDED

#include "keyval.h"

typedef struct node{
				struct node *next;
				char req[4];
				char val[25];
				//will add the socket later.
}node;

node *init();
void enque(node * head, char *req, char *val);
node *deque(node * head);
bool isEmpty(node * head);
char * submitRequest(int index, char *req, char *val);

#endif
