/*
 Author : Jayant Gupta
 Date : October 12, 2015
 This is the sub-code containing the 
 functions related to the dictionary.
*/

#include "keyval.h"
/* Data Structures*/
static char dict[100000][25]; // Data Structure storing the dictionary.
static bool flag[100000]={0};

/* Functions*/
void init_dict(const char *file_name);
bool _PUT(int key, char * value);
char * _GET(int key);
bool _DELETE(int key);

// Note: each string has a newline character at the end.
void init_dict(const char *file_name){
	FILE *fp;
	fp=fopen(file_name, "r");
	char c;
	if(fp == NULL){
		printf("Fatal Error:File does not exist\n");
		exit(EXIT_FAILURE);
	}
	const size_t line_size = 300;
	char *line = malloc(line_size);
	int index = 0;
	while(fgets(line, line_size, fp)!= NULL){
		strcpy(dict[index++], line);
	}
	free(line);
	int i;
	for(i = 0 ; i < index ; i++){
		flag[i] = true;
	}
	for(i = index ; i < 100000 ; i++){
		flag[i] = false;
	}
	return;	
}

bool _PUT(int key, char *value){
	printf("Inside _PUT\n");
	if(key >= 0 && key < 100000 ){
		if(flag[key]){
			printf("Key exists\n");
			printf("%s\n",dict[key]);
			return false;
		}
		else{
			strcpy(dict[key],value);
			flag[key] = true;
			printf("%s\n",dict[key]);
			printf("KEYVAL:: %d:%s\n", key, _GET(key));
			return true;
		}
	}
	else{
		printf("Invalid Key\n");
		return false;
	}
}

char *_GET(int key){
	if(key >= 0 && key < 100000 ){
		if(flag[key]){
			return dict[key];
		}
		else{
			printf("Value does not exist\n");
			return "Invalid Key";
		}
	}
	else{
		printf("Invalid Key\n");
		return "Invalid Key";
	}	
}

bool _DELETE(int key){
	if(key >= 0 && key < 100000 ){
		if(flag[key]){
			printf("_DELETING %d %s\n", key, dict[key]);
			flag[key] = false;
			return true;
		}
		else{
			printf("Value does not exist\n");
			return false;
		}
	}
	else{
		printf("Invalid Key\n");
		return false;
	}	
}