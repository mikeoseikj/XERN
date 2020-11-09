#include "include/stdio.h"
#include "include/string.h"
#include "include/unistd.h"

int main(int argc, char *argv[])
{
	char buf[61];
	if(argc < 2)
	{
		printf("error: %s: No file provided\n", argv[0]);
		return -1;
	}
	if(strcmp(argv[1], "-h") == 0)
	{
		printf("usage: %s <file>\n", argv[0]);
		return 0;
	}
	if(readlink(argv[1], buf) > 0)
		printf("%s\n", buf);

	return 0;
}
