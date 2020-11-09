#ifndef DUP_H
#define DUP_H

extern struct proc *curr_proc;
int kdup2(int oldfd, int newfd)
{
	if((oldfd < 0) || (newfd < 0))
		return -1;

	if(oldfd >= PROC_FD_MAX || newfd >= PROC_FD_MAX)
		return -1;

	if((curr_proc->fd[oldfd] == 0))
		return -1;

	if(curr_proc->fd[newfd])
		kclose(newfd);

	curr_proc->fd[newfd] = curr_proc->fd[oldfd];
	curr_proc->fd[newfd]->refcount++;
	
	return newfd;
}

#endif