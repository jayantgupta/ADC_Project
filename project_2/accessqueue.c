/*
	Author : Jayant Gupta
	Date : October 31, 2015
	Containing code for access
	queue utility.
	Dummy node implementation.
*/

#include "accessqueue.h"

int main(){

	return 0;
}

// Initializes by adding a dummy node to the start of the queue.
node *init(){
	node *head = (node *)malloc(sizeof(node));
	head->next=NULL;
	return head;
}

// Enques the values to the end of the queue.
void enque(node *head, char *req, char *val){
	node *temp = head;
	while(temp->next!=NULL){
		temp = temp->next;
	}
	temp->next=(node *)malloc(sizeof(node));
	strcpy(temp->next->req, req);
	strcpy(temp->next->val, val);
	temp->next->next = NULL;
	return;
}

// Deques the start element of the queue.
node * deque(node *head){
	if(!isEmpty(head)){
		node * start = head->next;
		head->next = head->next->next;
		return start;
	}
}

// Checks if the queue is empty
bool isEmpty(node *head){
	return (head->next == NULL);
}

char * submitRequest(int index, char *req, char *val){
	if(strcmp(req, "PUT") == 0){
		if(PUT(index, val)){
			return "PUT Success";
		}
		else return "PUT Failed";
	}
	else if(strcmp(req, "GET") == 0){
		char * value = GET(index);
		return value;
	}
	else if(strcmp(req, "DEL") == 0){
		if(DELETE(index)){
			return "DELETE Success";
		}
		else return "DELETE Failed";
	}
}
