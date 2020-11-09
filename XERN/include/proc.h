#ifndef PROC_H
#define PROC_H

#include "ext2.h"
#include "param.h"
#include "types.h"

enum procstate { UNUSED = 0, BLOCK, ZOMBIE, SLEEPING, RUNNABLE};

struct context
{
	unsigned int sp;
	unsigned int r0;
	unsigned int r1;
	unsigned int r2;
	unsigned int r3;
	unsigned int r4;
	unsigned int r5;
	unsigned int r6;
	unsigned int r7;
	unsigned int r8;
	unsigned int r9;
	unsigned int r10;
	unsigned int r11;
	unsigned int r12;
	unsigned int lr;
};


struct trapframe
{
	unsigned int r0;
	unsigned int r1;
	unsigned int r2;
	unsigned int r3;
	unsigned int r4;
	unsigned int r5;
	unsigned int r6;
	unsigned int r7;
	unsigned int r8;
	unsigned int r9;
	unsigned int r10;
	unsigned int r11;
	unsigned int r12;
	unsigned int lr;
	unsigned int sp;
    unsigned int cpsr;
};

/* eventhough the kernel is non-pre-emptive priority scheduling has been incorporated into alogorithms for possible future changes */
struct proc
{
	struct proc *next;			  
	struct context *context;     
	struct trapframe *tf;         
	unsigned int size;           
	unsigned int *pgdir;         
	enum procstate state;         
	int priority;               
	int pid, ppid;              
	struct proc *parent;          
	int event;                
	int exitcode;              
	int uid, gid;
	struct oft *fd[PROC_FD_MAX];
	struct minode *cwd;
	char name[256];
	int kstack[STACKSIZE];
	unsigned int brk_start;
	char loginname[65];
};

#endif
