#include<stdio.h>
#include<string.h>


int main(){
				char request[1024] = "";
				char key[10];
				printf("%s\n", request);
				strcat(request, " me!!");
				printf("%s\n", request);
				int a = 9;
				sprintf(key, "%d", a);
				strcat(request,key);
				printf("%s\n", request);
}
