#include "include/unistd.h"
#include "include/stdio.h"

int main(int argc, char *argv[])
{
	for(int i = 1; i < argc; i++)
		printf("%s ", argv[i]);
	putc('\n');
	return 0;
}