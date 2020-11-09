#include "include/stdio.h"
#include "include/unistd.h"

int main()
{
	char buf[65];
	getlogin(buf);
	printf("%s\n", buf);
}