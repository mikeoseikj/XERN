#include "include/stdio.h"
#include "include/string.h"
#include "include/unistd.h"

int main(int argc, char *argv[])
{
	if(argc < 4)
	{
		if((argv[1] != 0) && (strcmp(argv[1], "-h") == 0))
		{
			printf("usage: %s <-h> <old file>  <new file>  -- For hardlinks\n", argv[0]);
			printf("usage: %s <-s> <target>    <linkpath>  -- For symlinks\n", argv[0]);
			return 0;
		}
		printf("error: %s: Expects 3 arguments. Use <-h> for more information\n", argv[0]);
		return -1;
	}

	if(strcmp(argv[1], "-h") == 0)
		link(argv[2], argv[3]);
	else if(strcmp(argv[1], "-s") == 0)
		symlink(argv[2], argv[3]);
	else
		printf("error: Unknown option \'%s\'", argv[1]);
	return 0;
}
