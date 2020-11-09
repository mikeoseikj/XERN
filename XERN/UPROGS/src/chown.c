#include "include/stdio.h"
#include "include/stat.h"
#include "include/unistd.h"
#include "include/string.h"

int main(int argc, char *argv[])
{
	if(argc < 3)
	{
		if(argv[1] != 0 && strcmp(argv[1], "-h") == 0)
		{
			printf("usage: %s <UID:GID>  <file>  \nNote: (<UID:> and <:GID> is also valid)\n", argv[0]);
			return 0;
		}
		printf("error: %s: Expects 2 arguments. Use <-h> for more information\n", argv[0]);
		return -1;
	}

	char id_str[strlen(argv[1])+1];
	strcpy(id_str, argv[1]);

	char *s_uid ="", *s_gid ="";
	int uid, gid;

	for(int i = 0; i < strlen(id_str); i++)
	{
		if(id_str[i] == ':')
		{
			id_str[i] = 0;
			s_uid = &id_str[0];
			s_gid = &id_str[i+1];
			break;
		}
	}

	if(strlen(s_uid) == 0 && strlen(s_gid) == 0)
	{
		printf("Error: Wrong format in input. Use \'-h\' option for more info\n", argv[0]);
		return -1;
	}
	
    struct stat statbuf;
	if(strlen(s_uid) == 0)
	{
		if(stat(argv[2], &statbuf) < 0)
			return -1;

		uid = statbuf.st_uid;
		gid = atoi(s_gid);
	}
	else if(strlen(s_gid) == 0)
	{
		if(stat(argv[2], &statbuf) < 0)
			return -1;
        
        uid = atoi(s_uid);
		gid = statbuf.st_gid;
	}
	else
	{
		uid = atoi(s_uid);
		gid = atoi(s_gid);
	}

	chown(argv[2], uid, gid);
	return 0;
}