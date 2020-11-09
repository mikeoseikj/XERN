#include "include/unistd.h"
#include "include/stdio.h"

int main(int argc, char *argv[])
{
	if(argv[1] == 0)
		return 0;

	printf("%s\n", argv[1]);
	return 0;
}