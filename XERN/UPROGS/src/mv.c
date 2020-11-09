/*
	This program is exactly the same as the 'cp program'. The only difference is that this 'mv program' will delete the 
	src file at the end of all operations 
*/

#include "include/stdio.h"
#include "include/unistd.h"
#include "include/libgen.h"
#include "include/unistd.h"
#include "include/fcntl.h"
#include "include/stat.h"

char src[1025], dest[1025], filename[1025], buf[1025];


int main(int argc, char *argv[])
{
	if(argc < 4)
	{
		if(argv[1] != 0 && strcmp(argv[1], "-h") == 0)
		{
			printf("usage: %s  <src file>  <dest folder>  <--true>  -- Overwrite destination file if it exists\n", argv[0]);
			printf("usage: %s  <src file>  <dest folder>  <--false> -- Donot overwrite destination file if it exists\n", argv[0]);
		    return -1;
		}

		printf("error: %s: Expects 3 arguments -- Use <-h> option for more information\n", argv[0]);
		return -1;
	}


    strncpy(src, argv[1], 1024);
	strncpy(dest, argv[2], 767);
	char *overwrite = argv[3];

	// Append filename to destination directory
	basename(src, filename);
	filename[256] = 0;
	strcat(dest, "/");
	strcat(dest, filename); 
	

	struct stat statbuf;
    int fd1, fd2;

	if(stat(src, &statbuf) < 0)
		return -1;

	if(! S_ISREG(statbuf.st_mode))
	{
		printf("error: %s: Source file is not a regular file\n", argv[0]);
		return -1;
	}

	fd1 = open(src, O_RDONLY);
	if(fd1 < 0)
		return -1;

	
	if(strcmp(overwrite, "--true") == 0)
	{
		if(fexist(dest))
		{
			if(stat(dest, &statbuf) < 0)
				return -1;

			if(! S_ISREG(statbuf.st_mode))
			{
				printf("error: %s: Destination is not a regular file\n", argv[0]);
				return -1;
			}
		}

		fd2 = open(dest, O_WRONLY|O_CREAT);
		if(fd2 < 0)
			return -1;

	}
	else if(strcmp(overwrite, "--false") == 0)
	{
		if(fexist(dest))
		{
			printf("error: %s: Destination file already exists!\n", argv[0]);
			return -1;
		}
		fd2 = open(dest, O_WRONLY|O_CREAT);
		if(fd2 < 0)
			return -1;
	}
	else
	{
		printf("error: Unknown option \'%s\': use <-h> for more information\n", overwrite);
		return -1;
	}

	stat(src, &statbuf);
	int ino = statbuf.st_ino;
	memset(&statbuf, 0, sizeof(statbuf));
	stat(dest, &statbuf);

	if(ino == statbuf.st_ino)
	{
		printf("Warning: %s: source and destination files are the same\n", argv[0]);
		return -1;
	}
	
	int nbytes;
	while(1)
	{
		nbytes = read(fd1, buf, 1024);

		if(nbytes < 0)
			goto error;

		if(nbytes == 0)
			break;

		if(nbytes != write(fd2, buf, 1024))
			goto error;

	}
	unlink(src);  // Delete the 'src file'
	return 0;

	error:
		printf("error: moving %s to %s\n", argv[1], argv[2]);

	return -1;
}