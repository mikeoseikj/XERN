#ifndef SHELL_H
#define SHELL_H

// The 2 functions below are required becuase i use multidimensional buffers as my storage for string arrays
void setbuf_null(char buffer[][256], int row)
{
	buffer[row][0] = 0;
}
int isbuf_null(char buffer[][256], int row)
{
	if(buffer[row][0] == 0)
		return 1; // TRUE

	return 0; // FALSE
}


void rmldspaces(char input[], char output[])
{
	int count = 0;
	for(int i =0; i < strlen(input); i++)
	{
		if(input[i] == ' ')
			count++;
		else 
			break;
	}
	strcpy(output, (char *)input + count);

}

// spaces are not removed from strings in quotes. ie: "  abc  "
void remove_multiple_spaces(char *input, char *output)
{
	char tmp[strlen(input) + 1];
	rmldspaces(input, tmp);

	int count = 0, quote = 0;
	for(int i = 0; i < strlen(tmp); i++)
	{
		if(tmp[i] == '\"')
		{
			if(!(i > 0 && tmp[i-1] == '\\'))
				quote++;
		}
		output[count] = tmp[i];
		if((output[count] == tmp[i+1]) && (output[count] == ' ') && (quote % 2) == 0) // don't remove space if we are in a quote
			continue;
		count++;
	}
	output[count] = 0;

	if(strlen(output) > 1 && output[strlen(output)-1] == ' ')   //remove trailing space if any
		output[strlen(output)-1] = 0;

}

// This function also prepends '/bin/' if neccessary
int get_progname(char *input, char output[])
{
	char tmp[strlen(input) + 1];
	rmldspaces(input, tmp);
	int count = 0;
	for(int i = 0; i < strlen(tmp); i++)
	{
		if(tmp[i] == ' ')
			break;
		count++;
	}

	if(tmp[0] == '.' && tmp[1] == '/') // eg: ./myprog
	{
		if(count >= 255) count = 254;
	    strncpy(output, tmp, count);
	    return 1; // Meaning: No /bin/ added
	}
	
	if(count >= 250) count = 249;
	strcpy(output, "/bin/");
	strncpy(&output[5], tmp, count);
	return 2; // Meaning: /bin/ is added 
	
}

int get_pipes(char *input, char cmds[][256])
{
	char sto[strlen(input) + 1];
	char *str = &sto[0];
	strcpy(str, input);

    int quotes = 0;
	int pos = 0;
	for(int i = 0; i < strlen(str); i++)
	{
		if(str[i] == '\"')
			quotes++;

		if(str[i] == '|' && (quotes % 2) == 0)
		{
			str[i] = 0;
			rmldspaces(str, cmds[pos]);
			str += i+1;
			i = -1;

			if(strlen(cmds[pos]) == 0)
			{
				puts("error: unexpected '|' in input\n");
				return 0;
			}

			pos++;
		}
		if(pos == 15)
		{
			setbuf_null(cmds, pos);
			return 1;
		}			
	}
	if(strlen(str))
	{
		rmldspaces(str, cmds[pos]);
		setbuf_null(cmds, pos+1);
		return 1;
	}
	setbuf_null(cmds, pos);
	return 1;
}



int get_cmd_args(char *input, char words[][256])  // MAX arg for exec() is 16-1 (argv[0])
{
	char sto[strlen(input) + 1];
	char *str = &sto[0];
	strcpy(str, input);
	
	int pos = 0, quotes = 0;
	for(int i = 0; i < strlen(str); i++)
	{
		if(str[i] == '\\')
		{
			int success = 1;
			switch(str[i+1])
			{
				case 'f':
					str[i] = '\f';
					break;
				case 'r':
					str[i] = '\r';
					break;
				case 'n':
					str[i] = '\n';
					break;
				case 't':
					str[i] = '\t';
					break;
				case 'v':
					str[i] = '\v';
					break;
				case 'b':
					str[i] = '\b';
					break;
				case 'a':
					str[i] = '\a';
					break;
				case '\\':
					str[i] = '\\';
					break;
				case '"':
					str[i] = '\"';
					break;
				case '\'':
					str[i] = '\'';
					break;
				default:
					success = 0;
			}
			if(success)
			{
				// shift string to the left by one character;
				for(int j = i+1; j <= strlen(str); j++)	// +null byte
				str[j] = str[j+1];
				continue;
			}
			
		}
		if(str[i] == '\"')
		{
			str[i] = ' ';
			quotes++;
		}

		
		if(str[i] == ' ')
		{
			if((quotes % 2) == 1)  //  if a " has already been found
				continue;

			str[i] = 0;
			rmldspaces(str, words[pos]);
			str += i+1; 
			i = -1;

			if(strlen(words[pos]) == 0)
				continue;
			pos++;
		}

		if(pos == 14)
		{
			setbuf_null(words, pos);
			goto end;
		}
	}
	if(strlen(str))
	{
		rmldspaces(str, words[pos]);
		setbuf_null(words, pos+1);
		goto end;
	}

	setbuf_null(words, pos);
	end:
		if((quotes % 2) == 1)
		{
			puts("error: unexpected '\"' in input\n");
			return 0;
		}
		return 1;
}


void redirect(int oldfd, int newfd)
{
	if(oldfd != newfd)
	{
	    dup2(oldfd, newfd);
		close(oldfd);
	}
}

int char_count(char *str, char ch)
{
	int count = 0;
	for(int i = 0; i < strlen(str); i++)
	{
		if(str[i] == ch)
			count++;
	}
	return count;
}


char args[15][256];
void execute_pipeline(char pipecmds[][256], int pos, int in_fd)
{
	int fd, pid, status, pipefd[2];
	char progname[256];
	
	int state = get_progname(pipecmds[pos], progname);
	int off = (state == 1)? strlen(progname) : strlen(progname)-5;  // 1 = /bin/ wasn't prepended to program name and 2 = /bin/ was prepended
    
    /*	If no args is provided the length of 'args_str' will be 0 because it will point to null byte
    	This won't be a problem in the end because of the nature of the 'get_cmd_args()' and related functions functions
    */
    char *args_str = pipecmds[pos]+off;
	if(!get_cmd_args(args_str, args))
		return;
	
	if(isbuf_null(pipecmds, pos+1))  // IF IT IS THE LAST PIPED COMMAND?
	{
		redirect(in_fd, STDIN_FILENO);
		// Implementing I/O redirection
		for(int row = 0; !isbuf_null(args, row); row++)
		{
			if(strcmp(args[row], "<") == 0)
			{
				setbuf_null(args, row);
				if(isbuf_null(args, row+1))	// if a file was not provided for <
				{
					puts("error: unexpected \'<\' in input\n");
					return;
				}
				fd = open(args[row+1], O_RDONLY);
				if(fd < 0)
					return;

				redirect(fd, STDIN_FILENO);
				row = row + 1;
			}
			else if(strcmp(args[row], ">") == 0)
			{
				setbuf_null(args, row);
				if(isbuf_null(args, row+1)) // if a file was not provided for >
				{
					puts("error: unexpected \'>\' in input\n");
					return;
				}
				fd = open(args[row+1], O_CREAT|O_TRUNC|O_WRONLY);
				if(fd < 0)
					return;
				redirect(fd, STDOUT_FILENO);
				row = row + 1;
			}

		}	// End of I/O redirection

		char *argv[15] = {0};	// Note: exec() acceps char **argv 
		for(int i = 0; !isbuf_null(args, i); i++)
			argv[i] = args[i];


		if(strcmp(progname, "/bin/cd") == 0)
		{
			if(argv[0] == 0)
			{
				puts("error: cd: No directory path provided\n");
				return;
			}
			chdir(argv[0]);
			return;
		}

		if(strcmp(progname, "/bin/exit") == 0)
			exit(0);



		pid = fork();
		if(pid < 0)
		{
			puts("error: [Internal shell problem]\n");
			return;
		}
		if(pid > 0)
		{
			wait(&status);
		}
		else
		{
			if(exec(progname, argv) < 0)
			{
				printf("%s => cmd execution error\n", progname);
				exit(-1);
			}
		}
	}
	else     // IF IT IS NOT THE LAST PIPED COMMAND?
	{

        char *argv[15] = {0};	// Note: exec() acceps char **argv 
		for(int i = 0; !isbuf_null(args, i); i++)
			argv[i] = args[i];

		if(strcmp(progname, "/bin/cd") == 0)
		{
			if(argv[0] == 0)
			{
				puts("error: cd: No directory path provided\n");
				return;
			}
			if(chdir(argv[0]) < 0) 
				return;
		    execute_pipeline(pipecmds, pos+1, STDIN_FILENO);
		    return;
		}

		if(strcmp(progname, "/bin/exit") == 0)
			exit(0);


		pipe(pipefd);
		pid = fork();

		if(pid < 0)
		{
			puts("error: [Internal shell problem]\n");
			return;
		}
		if(pid == 0)
		{
			close(pipefd[0]);
			redirect(in_fd, STDIN_FILENO);
			redirect(pipefd[1], STDOUT_FILENO);
			
			// Implementing I/O redirection
			for(int row = 0; !isbuf_null(args, row); row++)
			{
				if(strcmp(args[row], "<") == 0)
				{
					setbuf_null(args, row);
					if(isbuf_null(args, row+1))	// if a file was not provided for <
					{
						puts("error: unexpected \'<\' in input\n");
						return;
					}
					fd = open(args[row+1], O_RDONLY);
					if(fd < 0)
						return;

					redirect(fd, STDIN_FILENO);
					row = row + 1;
				}
				else if(strcmp(args[row], ">") == 0)
				{
					setbuf_null(args, row);
					if(isbuf_null(args, row+1)) // if a file was not provided for >
					{
						puts("error: unexpected \'>\' in input\n");
						return;
					}
					fd = open(args[row+1], O_CREAT|O_TRUNC|O_WRONLY);
					if(fd < 0)
						return;
					redirect(fd, STDOUT_FILENO);
					row = row + 1;
				}

			}	// End of I/O redirection

			if(exec(progname, argv) < 0)
			{
				printf("%s => cmd execution error\n", progname);
				exit(-1);
			}

		}
		else
		{
			wait(&status);
			close(pipefd[1]);
			close(in_fd);
			execute_pipeline(pipecmds, pos+1, pipefd[0]);
			close(pipefd[0]);

		}
	}
}

#endif