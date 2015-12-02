#include<stdio.h>
#include<string.h>

int main(int argc, char **argv){
	char *ip[] = {"a", "ab","hello"};
	printf("%d\n", sizeof(ip)/sizeof(ip[0])==3);
	return 0;
}
