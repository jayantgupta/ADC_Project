#include<unistd.h>
#include<stdio.h>

int main(){
	execl("/bin/ping", "ping", "-c 1", "www.google.co.in", (char *)NULL);
	
}
