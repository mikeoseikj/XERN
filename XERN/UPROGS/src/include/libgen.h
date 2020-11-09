#ifndef LIBGEN_H
#define LIBGEN_H


void remove_multiple_slashes(char *input, char output[])
{
	int count = 0;
	for(int i = 0; i < strlen(input); i++)
	{
		output[count] = input[i];
		if((output[count] == input[i+1]) && (output[count] == '/'))
		{
			output[count] = input[i];
			continue;
		}

		count++;
	}
	output[count] = 0;

	if(strlen(output) > 1 && output[strlen(output)-1] == '/')   //remove trailing slash if any
		output[strlen(output)-1] = 0;
}


void basename(char *pathname, char base[])
{
	char path[strlen(pathname)+1];
	remove_multiple_slashes(pathname, path);

	if(strlen(path) == 1 && (path[0] == '/' || path[0] == '.'))
	{
		base[0] = path[0], base[1] = 0;
		return;
	}
	for(int i = (strlen(path)-1); i >= 0; i--)
	{
		if(path[i] == '/')
		{
			strncpy(base, &path[i + 1], strlen(path) - i);
			return;
		}

	}
	strcpy(base, path);
}

void dirname(char *pathname, char base[])
{
	char path[strlen(pathname)+1];
	remove_multiple_slashes(pathname, path);

    int contains_slash = 0;
	for(int i = 0; i < strlen(path); i++)
	{
		if(path[i] == '/')
			contains_slash++;
	}
	if(!contains_slash)
	{
		base[0] = '.', base[1] = 0;
		return;
	}

	if(strlen(path) == 1 && (path[0] == '/' || path[0] == '.'))
	{
		base[0] = path[0], base[1] = 0;
		return;
	}
	for(int i = (strlen(path)-1); i >= 0; i--)
	{
		if(path[i] == '/')
		{
			if(i == 0) i = 1;
			strncpy(base, path, i);
			break;
		}
	}
}

#endif