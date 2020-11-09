#include "include/globals.h"
#include "include/types.h"

extern unsigned int return_to_usermode();
int kfork()
{
	struct proc *np = allocproc();
	if(! np)
		return -1;

	if(! copyuvm(np, curr_proc))
    {
        dallocproc(np);
		return -1;
    }
    
    np->size = curr_proc->size;
    np->brk_start = curr_proc->brk_start;
    np->state = RUNNABLE;
	np->parent = curr_proc;
	np->priority = curr_proc->priority;
	np->ppid = curr_proc->pid;
	np->uid = curr_proc->uid;
	np->gid = curr_proc->gid;
    np->cwd = curr_proc->cwd;
    strncpy(np->name, curr_proc->name, 255);
    strncpy(np->loginname, curr_proc->loginname, 64);

    np->context->sp = &(np->kstack[STACKSIZE - 30]);  // exclude context->sp

    /* Setting kernel register 'stack' to zeros */
    np->context->r0 = np->context->r1 = np->context->r2 = 0; 
    np->context->r3 = np->context->r4 = np->context->r5 = 0;
    np->context->r6 = np->context->r7 = np->context->r8 = 0;
    np->context->r9 = np->context->r10 = np->context->r11 = 0;
    np->context->r12 = 0;

    np->context->lr = (unsigned int)return_to_usermode;

    /* Setting userspace registers 'stack' to zeros */
    np->tf->r1 = np->tf->r2 =  np->tf->r3 = np->tf->r4 = np->tf->r5 = 0;
    np->tf->r6 = np->tf->r7 = np->tf->r8 = np->tf->r9 = np->tf->r10 = 0;
    np->tf->r11 = np->tf->r12 = 0;
    
    np->tf->r0 = 0;	//return zero in child
    np->tf->lr = curr_proc->tf->lr;
    np->tf->sp =  curr_proc->tf->sp;  
    np->tf->cpsr = curr_proc->tf->cpsr; 

    for(int i = 0; i < PROC_FD_MAX; i++)
    {
    	np->fd[i] = curr_proc->fd[i];

    	if(np->fd[i] != 0)
    	{
    		np->fd[i]->refcount++;

    		if(np->fd[i]->mode == READ_PIPE)
    			np->fd[i]->p_ptr->nread++;

    	    else if(np->fd[i]->mode == WRITE_PIPE)
    			np->fd[i]->p_ptr->nwrite++;
    	}
    }
    
    enqueue(&runnablequeue, np);
    return np->pid;   // return childs pid in parent    
}

