#ifndef KEYVAL_H_INCLUDED 
#define KEYVAL_H_INCLUDED

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

void init_dict(const char * file_name);
bool _PUT(int key, char *value);
char * _GET(int key);
bool _DELETE(int key);
#endif
