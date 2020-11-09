#include "include/stdio.h"
#include "include/string.h"
#include "include/unistd.h"

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("usage: %s <pid>\n", argv[0]);
		return -1;
	}

	kill(atoi(argv[1]));
}