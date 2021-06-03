#include "include/unistd.h"
#include "include/process.h"
#include "include/stdio.h"

struct process storage[64];		// too large to be stored on the stack
int main()
{
	struct process *procs[64];
	for(int i = 0; i < 64; i++)
		procs[i] = &storage[i];

	int count = get_procs(procs);
	printf(" PID       STATE         MEMORY        PROGRAM\n");		
	for(int i = 0; i < count; i++)
	{
		printf("  %d        ", procs[i]->pid);
		if(procs[i]->state)
			printf("running      ");
		else
			printf("inactive     ");

		printf(" %d         %s\n", procs[i]->size, procs[i]->name);
	}
}
