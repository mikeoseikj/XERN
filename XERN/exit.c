#include "include/globals.h"

void kexit(int value)
{
    int w_childs = 0;
    if(curr_proc->pid == 1)
    {
        kprintf("Init process can't be exited\n");
        return;
    }
    for(int i = 0; i < PROC_FD_MAX; i++)
    {
        if(curr_proc->fd[i] != 0)
            kclose(i);
    }

    for(int i = 2; i < NPROCS; i++) //Note: process 0 is not used
    {
        struct proc *p = &procs[i];
        if((p->state != UNUSED) && (p->ppid == curr_proc->pid)) // Check for orphaned process
        {
            p->ppid = 1;
            p->parent = &procs[1];
            w_childs++;
        }
    }
    curr_proc->exitcode = value;
    curr_proc->state = ZOMBIE;
    wakeup((int)curr_proc->parent);
    
    if(w_childs)    // Wakeup init process if there are orphanned childs
        wakeup((int)&procs[1]);  
    switch_context();

}
