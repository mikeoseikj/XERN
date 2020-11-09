#include "include/stdio.h"
#include "include/unistd.h"
#include "include/string.h"

int main(int argc, char *argv[])
{
	if(argc < 3)
	{

		if(argv[1] != 0 && strcmp(argv[1], "-h") == 0)
		{
			printf("usage: %s  <file/folder>  <9 bytes permission(eg: r-xrwxrw-)>\n", argv[0]);
			return 0;
		}
		printf("error: %s: permissions wasn't provided. Use \'-h\' for more information\n", argv[0]);
		return -1;
	}

	char *permission = argv[2];

	if(strlen(permission) < 9)
	{
		printf("error: %s: length of permission string not up to 9 bytes\n", argv[0]);
		return -1;
	}


	for(int i = 0; i < 9; i++)
	{
		if(permission[i] != 'r' && permission[i] != 'w' && permission[i] != 'x' && permission[i] != '-')
		{
			printf("error: %s: Wrong values in permission string.\n", argv[0]);
			return  -1;
		}
	}

	/*
		Values for permission of either user, group and others can be repeated or mixed. This won't cause a problem.
		Eg: rrxrrrwww , wrxxrwr--, etc is accepted
	*/

    unsigned short mode = 0;
	for(int i = 0; i < 3; i++)
	{
		if(permission[i] == 'r')
			mode |= (1<<8);
		else if(permission[i] == 'w')
			mode |= (1<<7);
		else if(permission[i] == 'x')
			mode |= (1<<6);
	}
	for(int i = 3; i < 6; i++)
	{
		if(permission[i] == 'r')
			mode |= (1<<5);
		else if(permission[i] == 'w')
			mode |= (1<<4);
		else if(permission[i] == 'x')
			mode |= (1<<3);
	}
	for(int i = 6; i < 9; i++)
	{
		if(permission[i] == 'r')
			mode |= (1<<2);
		else if(permission[i] == 'w')
			mode |= (1<<1);
		else if(permission[i] == 'x')
			mode |= 1;
	}

	chmod(argv[1], mode);
}