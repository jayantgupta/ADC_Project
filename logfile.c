/*
 Date : October 20, 2015
 This is the sub-code containing server and client side logging functionality.
*/

   #include<stdio.h>
// #include<stdlib.h>
// #include<string.h>
// #include<stdbool.h>
   #include <time.h>

#include "logfile.h"


/* Functions*/
void callLog(const char *message,const char *file_name);

void callLog(const char *message,const char *file_name){
	FILE *fp;
	fp=fopen(file_name, "a");
        time_t rawtime;
        struct tm * timeinfo;
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );

	if(fp == NULL){
		printf("Fatal Error:File does not exist\n");
		exit(EXIT_FAILURE);
	}
        fprintf(fp, "%s : %s \n", asctime(timeinfo),message); 
        fclose(fp);

}



