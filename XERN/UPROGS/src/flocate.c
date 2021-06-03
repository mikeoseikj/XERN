#include "include/stdio.h"
#include "include/unistd.h"
#include "include/string.h"
#include "include/stat.h"
#include "include/fcntl.h"


void find_patterns(char *root, char *pattern)
{
	char *path = sbrk(1); // We will use around 3KiB -- malloc() has not been implemented yet
	int fd = open(root, O_DIRECTORY);
	if(fd < 0)
		return;

	struct dirent dir_entry;
	struct stat statbuf;

	while(getdent(fd, &dir_entry) > 0)
	{
		if(strcmp(dir_entry.d_name, ".") == 0 || strcmp(dir_entry.d_name, "..") == 0 || strcmp(dir_entry.d_name, "root") == 0)
			continue;

		strncpy(path, root, 2048);
		strcat(path, "/");
		strcat(path, dir_entry.d_name);

		if(stat(path, &statbuf) < 0)
			return;

		char *out = path;
		if(out[0] == '.' && out[1] == '/')
				out += 2;

		if(S_ISDIR(statbuf.st_mode))
		{
			
			if(strstr(out, pattern))
				printf("%s\n", out);

			find_patterns(path, pattern);

		}
		else
		{
			
			if(strstr(out, pattern))
				printf("%s\n", out);
		}
		memset(path, 0, strlen(path));
	}
	close(fd);

}

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("usage: %s  <pattern>\n", argv[0]);
		return -1;
	}
	char *root = "/root";
	find_patterns(root, argv[1]);

}
