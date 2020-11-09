#include "include/stdio.h"
#include "include/string.h"
#include "include/unistd.h"

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("error: %s: No path provided\n", argv[0]);
		return -1;
	}
	if(strcmp(argv[1], "-h") == 0)
	{
		printf("usage: %s <path>\n", argv[0]);
		return 0;
	}
	rmdir(argv[1]);

	return 0;
}
