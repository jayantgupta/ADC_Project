#include<string.h>
#include "global.h"

int main(int argc, char **argv){
	char *ip[] = {"a", "ab","hello"};
	printf("%d\n", sizeof(ip)/sizeof(ip[0])==3);
	check(5);
	check(6);
	return 0;
}
