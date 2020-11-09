#ifndef QUEUE_H
#define QUEUE_H

#include "proc.h"

struct proc *allocproc()
{
    struct proc *p = freequeue;
    if(p)
        freequeue = p->next;

    return p;
}


void dallocproc(struct proc *p)
{
    p->next = freequeue;
    freequeue = p;
}


void enqueue(struct proc **queue, struct proc *p)
{
    struct proc *q  = *queue;
    if(q == 0 || p->priority > q->priority)
    {
        *queue = p;
        p->next = q;
        return;
    }
    while (q->next && (p->priority <= q->next->priority))
        q = q->next;

    p->next = q->next;
    q->next = p;
}

struct proc *dequeue(struct proc **queue)
{
    struct proc *p = *queue;
    if(p)
        *queue = p->next;

    return p;
}

#endif