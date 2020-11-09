#include "include/globals.h"

extern struct minode root_inode;

void scheduler()
{	
	struct proc *old_proc = curr_proc;
	if(curr_proc->state == RUNNABLE)
		enqueue(&runnablequeue, curr_proc);

	curr_proc = dequeue(&runnablequeue);

	if(curr_proc->pgdir != old_proc->pgdir)
		switchuvm((unsigned int)curr_proc->pgdir);
}


void OS_init()
{
	struct proc *p;
	for(int i = 0; i < NPROCS; i++)
	{
		p = &procs[i];
		p->context = &(p->kstack[STACKSIZE - 31]);   /* Points to kernel mode registers (check 'proc.h') */
		p->tf = &(p->kstack[STACKSIZE - 16]);        /*Points to the usermode registeres (check 'proc.h')*/
		p->pid = i;
		p->state = UNUSED;
		p->priority = 0;
		p->ppid = 0;
		p->parent = 0;
		p->size = 0;
		p->uid = 0;
		p->gid = 0;
		p->cwd = &root_inode;
		p->next = p + 1;
		p->pgdir = (int *)(0x600000 + (p->pid * 0x4000));

		for(int j = 0; j < PROC_FD_MAX; j++)
			p->fd[i] = 0;
	}
	procs[NPROCS-1].next = 0;
	freequeue = &procs[0];
	runnablequeue = 0;
	sleepqueue = 0;

	curr_proc = allocproc();	// Will be used as temporal storage for virtual memory pages in fork()_
	curr_proc->priority = 0;
	curr_proc->state = RUNNABLE;

    pipe_init();
	setupvm();
}

