#include "include/stdio.h"
#include "include/unistd.h"

int main()
{
	char cwd[1025] = {0};
	getcwd(cwd, 1025);
	printf("%s\n", cwd);
	return 0;
}