#ifndef SYNC_H
#define SYNC_H

#include "proc.h"
#include "globals.h"
#include "queue.h"


void sleep(int event)
{
    curr_proc->event = event;
    curr_proc->state = SLEEPING;
    enqueue(&sleepqueue, curr_proc);
    switch_context();
}

void wakeup(int event)
{
    struct proc *p, *tmp = 0;
    while((p = dequeue(&sleepqueue)) != 0)
    {
        if(p->event == event)
        {   
            p->state = RUNNABLE;
            enqueue(&runnablequeue, p);
        }
        else
        {
            enqueue(&tmp, p);
        }
    }
    sleepqueue = tmp;
}

#endif 
