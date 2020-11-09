#ifndef PWD_H
#define PWD_H

#include "string.h"

struct passwd  // All buffers has space for null
{
	char pw_name[33]; 		/* username */
	char pw_passwd[33];     /* user password */
	int  pw_uid;            /* user ID */
	int  pw_gid;            /* group ID*/
	char pw_gecos[65];      /* user information */
	char pw_dir[65];        /* home directory */
	char pw_shell[65];      /* shell program */
};

int pw_char_count(char *str, char ch)
{
	int count = 0;
	for(int i = 0; i < strlen(str); i++)
	{
		if(str[i] == ch)
			count++;
	}
	return count;
}

int getpwd(int fd, struct passwd *creds)
{
	char buf[300];
	char *ptr = &buf[0];
	char *pbuf = &buf[0];
	int sep = 0;

	if(readline(fd, ptr, 300) > 0)
	{
		if(pw_char_count(ptr, ':') != 6)  // wrong format
			return -1;

        int len = strlen(buf);
		for(int i = 0; i < len; i++)
		{
			if(*ptr == ':')
			{
				*ptr = 0;
				sep++;
				if(strlen(pbuf) == 0)	// Wrong format -- That field wasn't provided
					return -1;

				switch(sep)
				{
					case 1:
						strncpy(creds->pw_name, pbuf, 32);
						break;

					case 2:
						strncpy(creds->pw_passwd, pbuf, 32);
						break;

					case 3:
						creds->pw_uid = atoi(pbuf);
						break;

					case 4:
						creds->pw_gid = atoi(pbuf);
						break;

					case 5:
						strncpy(creds->pw_gecos, pbuf, 64);
						break;

					case 6:
						strncpy(creds->pw_dir, pbuf, 64);
						break;
				}
				pbuf = ptr + 1;
				ptr += strlen(ptr); // incremented by 1 by ptr++;
			}

			ptr++;
			if(sep == 6)
			{
				strncpy(creds->pw_shell, pbuf, 64);
				return 0;   //SUCCESS
			}
		}

	}
	return -1;
}

#endif