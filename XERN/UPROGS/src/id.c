#include "include/stdio.h"
#include "include/unistd.h"

int main()
{
	int uid = getuid();
	int gid = getgid();

	char username[65];
	getlogin(username);

	if(gid == 0)
		printf("uid=%d(%s)  gid=%d(root)\n", uid, username, gid);
	
	else if(gid == uid)
		printf("uid=%d(%s)  gid=%d(%s)\n", uid, username, gid, username);
	
	else
		printf("uid=%d(%s)  gid=(%d)\n", uid, username, gid);


	
}