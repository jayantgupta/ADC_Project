#include "keyval.h"

int main(int argc, char **argv){
	const char *name = "word_list";
	init_dict(name);
	printf("%s", GET(9));
	printf("%s\n", PUT(9, "TEST") ? "true" : "false");
	printf("%s\n", DELETE(9) ? "true" : "false");
	printf("%s\n", PUT(9, "TEST") ? "true" : "false");
	printf("%s\n", GET(9));
}
