#include "include/fcntl.h"
#include "include/stdio.h"
#include "include/unistd.h"
#include "include/string.h"
#include "include/pwd.h"

int main()
{
	struct passwd creds = {0};
	char user[33], pass[33];

	int fd = open("/etc/passwd", O_RDONLY);
	if(fd < 0)
		exit(-1);



	puts("login: ");
	gets(user, 32);
	puts("Password: ");
	readpass(pass, 32);

	while(getpwd(fd, &creds)  == 0)
	{
				
		if(strcmp(user, creds.pw_name) == 0 && strcmp(pass, creds.pw_passwd) == 0)
		{
			setlogin(creds.pw_name);
			setuid(creds.pw_uid);
			setgid(creds.pw_gid);
			chdir(creds.pw_dir);
			if(exec(creds.pw_shell, 0) == 0)
			{
				puts("[*] Failed to start user shell program\n");
				exit(-1);
			}
		}		
	}
	puts("\nLogin incorrect\n");
}
