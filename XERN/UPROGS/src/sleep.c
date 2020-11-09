#include "include/stdio.h"
#include "include/unistd.h"
#include "include/string.h"

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("%s  <seconds>\n", argv[0]);
		return -1;
	}

	int seconds = atoi(argv[1]);
	sleep(seconds);
}