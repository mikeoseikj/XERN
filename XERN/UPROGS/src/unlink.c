#include "include/stdio.h"
#include "include/string.h"
#include "include/unistd.h"

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("error: %s: No filename provided\n", argv[0]);
		return -1;
	}
	if(strcmp(argv[1], "-h") == 0)
	{
		printf("usage: %s <file>\n", argv[0]);
		return 0;
	}
	unlink(argv[1]);

	return 0;
}
