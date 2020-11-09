#include "include/stdio.h"
#include "include/fcntl.h"
#include "include/string.h"
#include "include/unistd.h"
#include "sh.h"

char line[1025], c_line[1025]; 
char piped_cmds[17][256];

int main(int argc, char *argv[])
{
	int dev_stdin = open("/dev/tty0", O_RDONLY);
	int dev_stdout = open("/dev/tty0", O_WRONLY);

	if(dev_stdin < 0 || dev_stdout < 0)
		return -1;

	char cwd[1025], loginname[65];
	getlogin(loginname);
	char sh = (getuid() == 0)? '#' : '$';
	for(;;)
	{
		if(getcwd(cwd, 1025) == 0)
		{
			printf("ERROR: [Internal shell error (specific with getcwd)]");
			exit(-1);
		}
		printf("\x1B[1;31m%s@xernhost\x1B[0m:\x1B[1;34m%s\x1B[0m%c ", loginname, cwd, sh);
		gets(line, 1024);

		remove_multiple_spaces(line, c_line);  // The use of this function simplifies implementation of the shell
        if(c_line[0] == 0)
        	continue;
 
        if(strncmp(c_line, "clear", 5) == 0)
        {
        	puts("\x1B[2J\x1B[H");
        	continue;
        }

		if(strstr(c_line, "\"\"") || strstr(c_line, "\" \"") || (char_count(c_line, '\"') % 2) != 0)
		{
			puts("error: unexpected '\"' in input\n");
			continue;
		}
		if(strstr(c_line, "||") || strstr(c_line, "| |"))
		{
			puts("error: unexpected '|' in input\n");
			continue;
		}

        int first_apos = 0;
		for(int i = 0; i < strlen(c_line)-1; i++) // execluding last char. it might be a '"'
		{
			if(c_line[i] == '\"' && first_apos == 0)
			{
				first_apos++;
				continue;
			}

			if(c_line[i] == '\"' && first_apos)
			{
				if(c_line[i+1] != ' ')
				{
					puts("error: unexpected '\"' in input\n");
					first_apos = -1;
					break;
				}
				first_apos = 0;
			}
		}
		if(first_apos == -1)
			continue;
       
		get_pipes(c_line, piped_cmds);

		execute_pipeline(piped_cmds, 0, STDIN_FILENO);
		dup2(dev_stdin, STDIN_FILENO);  
		dup2(dev_stdout, STDOUT_FILENO); 
	}

}
