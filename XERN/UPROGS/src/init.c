
#include "include/fcntl.h"
#include "include/dirent.h"
#include "include/unistd.h"

int main()
{
	int status, pid, which;
	char buf[20];

	int stdin = open("/dev/tty0", O_RDONLY);   
	int stdout = open("/dev/tty0", O_WRONLY);
	int stderr = open("/dev/tty0", O_WRONLY);


	if(stdin < 0 || stdout < 0 || stderr < 0)
		while(1);

  loop:
		pid = fork();
		if(pid)
		{
			which = wait(&status);
			if(which == pid)
				goto loop;
		}
		else
		{
			exec("/bin/login", 0);
		}		
}
