#include "include/globals.h"

int kwait(int *status)
{
    struct proc *p;
    int child = 0;
   
    for(int i = 1; i < NPROCS; i++)
    {
        p = &procs[i];
        if((p->state != UNUSED) && (p->ppid == curr_proc->pid))
            child++;
    }
    if(child == 0)
        return -1;
   
    while(1)
    {
        for(int i = 2; i < NPROCS; i++)
        {
            p = &procs[i];        
            if((p->state == ZOMBIE) && (p->ppid == curr_proc->pid))
            {
                *status = p->exitcode;
                p->state = UNUSED;
                freeuvm(p);
                dallocproc(p);
                return p->pid;
            }
        }
        sleep((int)curr_proc); 
    }   
}



