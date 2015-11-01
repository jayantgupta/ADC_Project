#ifndef KEYVAL_H_INCLUDED 
#define KEYVAL_H_INCLUDED

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

void init_dict(const char * file_name);
bool PUT(int key, char *value);
char * GET(int key);
bool DELETE(int key);
#endif
