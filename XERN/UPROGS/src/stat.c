#include "include/stdio.h"
#include "include/string.h"
#include "include/unistd.h"
#include "include/stat.h"

void digit_format_spaces(int value)
{
	int len = 1;
    /* Dynamically decide on number of spaces to display after a number (for formatting reason) */
    while(value >= 10)
    {
    	len++;
    	value/=10;
    }
    len = 10-len; 
    for(int i = 0; i < len; i++)
    	putc(' ');
}

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("error: %s: No file or folder provided\n", argv[0]);
		return -1;
	}
	if(strcmp(argv[1], "-h") == 0)
	{
		printf("usage: %s  <file/folder>\n", argv[0]);
		return 0;
	}

	struct stat statbuf;
	if(stat(argv[1], &statbuf) < 0)
		return -1;

	printf("\n\x1B[1;32mFile:\x1B[0m %s\n", argv[1]);
	printf("\x1B[1;32mSize:\x1B[0m %d ", statbuf.st_size, statbuf.st_blocks);
	digit_format_spaces(statbuf.st_size);
	printf("\x1B[1;32mBlocks:\x1B[0m %d", statbuf.st_blocks);
	digit_format_spaces(statbuf.st_blocks);
	printf("\x1B[1;32mIO Blocks:\x1B[0m %d", statbuf.st_blksize);
	digit_format_spaces(statbuf.st_blksize);
	puts("\x1B[1;32mType:\x1B[0m ");


	if(S_ISREG(statbuf.st_mode))
		puts("regular file\n");
	else if(S_ISDIR(statbuf.st_mode))
		puts("directory\n");
	else if(S_ISCHR(statbuf.st_mode))
		printf("char device (maj: %d  min: %d)\n", (statbuf.st_rdev & 0xFF00) >> 8, statbuf.st_rdev & 0xFF);
	else if(S_ISBLK(statbuf.st_mode))
		printf("block device (maj: %d  min: %d)\n", (statbuf.st_rdev & 0xFF00) >> 8, statbuf.st_rdev & 0xFF);
	else if(S_ISLNK(statbuf.st_mode))
		puts("symbolic link\n");
	else 
		puts("unknown file type\n");

	printf("\x1B[1;32mLinks:\x1B[0m %d", statbuf.st_nlink);
	digit_format_spaces(statbuf.st_nlink);
	printf("\x1B[1;32mInode:\x1B[0m %d ", statbuf.st_ino);
	digit_format_spaces(statbuf.st_ino);
	printf("\x1B[1;32mUid:\x1B[0m (%d)    ", statbuf.st_uid);
	digit_format_spaces(statbuf.st_uid);
	printf("\x1B[1;32mGid:\x1B[0m (%d)\n\n", statbuf.st_gid);

	return 0;
}