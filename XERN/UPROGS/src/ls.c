#include "include/stdio.h"
#include "include/stat.h"
#include "include/dirent.h"
#include "include/fcntl.h"
#include "include/string.h"
#include "include/unistd.h"

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

void print_info(char *path, char *file)
{
	if(file[0] == '.') // Won't show '.' and '..' directories and also hidden files (file that begins with '.')
		return; 

	struct stat statbuf;
	if(stat(path, &statbuf) < 0)
		return;

    if(S_ISLNK(statbuf.st_mode))
    	printf("\x1B[1;36m%s  \x1B[0m", file);  // Color => cyan

    else if(S_ISDIR(statbuf.st_mode))
    	printf("\x1B[1;34m%s  \x1B[0m", file); // Color => blue

    else if(S_ISCHR(statbuf.st_mode) || S_ISBLK(statbuf.st_mode)) 
        printf("\x1B[1;33m%s  \x1B[0m", file); // Color => yellow
  
    else
    	printf("\x1B[1m%s\x1B[0m  ", file);
}

void print_detailed_info(char *path, char *file)
{
	struct stat statbuf;

	if(stat(path, &statbuf) < 0)
		return;

	if(S_ISDIR(statbuf.st_mode))
		putc('d');
	else if(S_ISLNK(statbuf.st_mode))
		putc('l');
	else if(S_ISBLK(statbuf.st_mode))
		putc('b');
	else if(S_ISCHR(statbuf.st_mode))
		putc('c');
	else
		putc('-');

    unsigned short mask = 000400;
    for(int i = 0; i < 3; i++)
    {
		(statbuf.st_mode & mask)? putc('r') : putc('-');
      	mask = mask >> 1;

     	(statbuf.st_mode & mask)? putc('w') : putc('-');
        mask = mask >> 1;

    	(statbuf.st_mode & mask)? putc('x') : putc('-');
        mask = mask >> 1;

    }

    printf("   %d", statbuf.st_nlink);
    digit_format_spaces(statbuf.st_nlink);
    printf("%d", statbuf.st_uid);
    digit_format_spaces(statbuf.st_uid);
    printf("%d", statbuf.st_gid);
    digit_format_spaces(statbuf.st_gid);
    printf("%d", statbuf.st_size);
    digit_format_spaces(statbuf.st_size);
   
    if(S_ISLNK(statbuf.st_mode))
    	printf("\x1B[1;36m%s\x1B[0m", file);  // Color => cyan

    else if(S_ISDIR(statbuf.st_mode))
    	printf("\x1B[1;34m%s\x1B[0m", file); // Color => blue

    else if(S_ISCHR(statbuf.st_mode) || S_ISBLK(statbuf.st_mode)) 
        printf("\x1B[1;33m%s\x1B[0m", file); // Color => yellow
  
    else
    	printf("\x1B[1m%s\x1B[0m", file);

    //print the symlinked file
    if(S_ISLNK(statbuf.st_mode))
    {
    	char link[61];
    	readlink(path, link);
    	printf(" -> %s", link);
    }
    putc('\n');

}

int main(int argc, char *argv[])
{
	int fd;
	char file_path[1025], buf[257];
	struct dirent entry;
	if(argc >= 2)
	{
		if(strcmp(argv[1], "-h") == 0)
		{
			printf("usage: %s <-l (Optional)>  <path (Optional)>\n", argv[0]);
			return 0;
		}
		if(strcmp(argv[1], "-l") == 0)
		{

			if(argv[2] == 0)  // if no arg was provided for option -l
			{
				strcpy(buf, "./");
			}
			else
			{
				strcpy(buf, argv[2]);
				strcpy(buf+strlen(buf), "/");
			}
			fd = open(buf, O_DIRECTORY);
			if(fd < 0)
				return -1;
     
			while(getdent(fd, &entry) > 0)
			{
				strcpy(file_path, buf);
				strcpy(file_path+strlen(file_path), entry.d_name);
				print_detailed_info(file_path, entry.d_name); // file_path is for the stat(), it need full path to file
			}

		    return 0;
		}

        // If the '-l' option wasn't provided
        strcpy(buf, argv[1]);
        strcpy(buf+strlen(buf), "/");
		fd = open(buf, O_DIRECTORY);
		if(fd < 0)
			return -1;
     
		while(getdent(fd, &entry) > 0)
		{
			strcpy(file_path, buf);
			strcpy(file_path+strlen(file_path), entry.d_name);
			print_info(file_path, entry.d_name);	// file_path is for the stat(), it need full path to file
		}
		putc('\n');
		return 0;

	}

    // If no cmds parameters was provided, list current directory
	fd = open(".", O_DIRECTORY);
	if(fd < 0)
		return -1;
     
	while(getdent(fd, &entry) > 0)
		print_info(entry.d_name, entry.d_name);
	putc('\n');
	return 0;

}
