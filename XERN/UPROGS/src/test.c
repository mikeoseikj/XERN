#include "include/fcntl.h"
#include "include/unistd.h"
#include "include/stdio.h"

int main(int argc, char *argv[])
{

	printf("argc => %d\n", argc);
	
	for(int i  = 0; argv[i]; i++)
		printf("argv[%d] => %s\n", i, argv[i]);

	for(int i = 0; i < 5; i++)
		printf("hello, world!\n");

}