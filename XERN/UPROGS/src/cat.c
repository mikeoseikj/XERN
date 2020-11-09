#include "include/stdio.h"
#include "include/fcntl.h"
#include "include/unistd.h"

int main(int argc, char *argv[])
{
	char buf[1025];
	if(argv[1] == 0)
	{
		gets(buf, 1024);
		printf("%s\n", buf);
		return 0;
	}

	int fd = open(argv[1], O_RDONLY);
	if(fd < 0)
		return -1;

	while(read(fd, buf, 1024) > 0)
	{
		printf("%s", buf);
	} 
	putc('\n');
	close(fd);
	return 0;
}
