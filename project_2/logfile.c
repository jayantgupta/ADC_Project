/*
Date : October 20, 2015
This is the sub-code containing server and client side logging functionality.
 */
#define _POSIX_C_SOURCE 200809L
#include "logfile.h"
#include <inttypes.h>
#include <math.h>

void callLog(const char *message,const char *file_name){
				// Getting the time.
				FILE *fp;
				char st[40], ms_str[3];
				long ms;
				time_t s; //rawtime;
				struct timespec spec;
				clock_gettime(CLOCK_REALTIME, &spec);
				s = spec.tv_sec;
				ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds
				char *time_string = ctime(&s);
				struct tm *p = localtime(&s);
				strftime(st, 1000, "%a %b-%d-%Y %k:%M:%S:", p);
				sprintf(ms_str,"%03ld", ms); 
				strcat(st, ms_str);
			  /* Opening and adding data to log file */	
				fp=fopen(file_name, "a");
				if(fp == NULL){
								printf("Fatal Error:File does not exist\n");
								exit(EXIT_FAILURE);
				}
			  fprintf(fp, "%s : %s",st ,message); 
				fclose(fp);
}
