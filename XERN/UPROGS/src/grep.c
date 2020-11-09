#include "include/stdio.h"
#include "include/unistd.h"
#include "include/fcntl.h"
#include "include/string.h"


void grep(int fd, char *pattern)
{
	char line[1025];
	while(readline(fd, line, 1024) >= 0)
	{
		if(strstr(line, pattern))
			printf("%s\n", line);
	}
}


// This function is needed because input might be piped
int bufsize, off = 0;
int buf_readline(char *input,  char *buf, int len)
{
    char c;

    for(int i = 0; i < len; i++)
    {
        c = input[off];
        if(off == bufsize && i > 0)  // End of buffer
        {
        	buf[i] = 0;
        	return i;
        }
        else if(off == bufsize)
        	return -1;         // No data read

        if(c == '\n')
        {
        	off++;
            buf[i] = 0;
            return i;
        }
        buf[i] = c;
        off++;

    }
    return -1;
}



int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("usage: %s  <pattern>  [FILE ...]\n", argv[0]);
		return -1;
	}

	if(argc == 2)
	{
		char buf[8193], line[1025];
		char *pattern = argv[1];

		bufsize = read(0, buf, 8192);
		if(bufsize < 0)
			return -1;

		while(buf_readline(buf, line, 1024) >= 0)
		{
			if(strstr(line, pattern))
				printf("%s\n", line);
		}

		return 0;
	}

    int fd;
	for(int i = 2; i < argc; i++)
	{
		fd = open(argv[i], O_RDONLY);
		if(fd < 0)
			return -1;

		printf("\x1B[1;32mFILE -- %s\x1B[0m\n", argv[i]);
		grep(fd, argv[1]);
		puts("\n");
		close(fd);
	}
	return 0;
}