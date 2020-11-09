#ifndef GLOBALS_H
#define GLOBALS_H

#include "types.h"

struct proc *runnablequeue, *sleepqueue, *freequeue;
struct proc *curr_proc;  
struct proc procs[NPROCS];
struct minode minodes[NMINODES];
struct oft open_file_table[NOFTS];
struct pipe pipes[NPIPES];

#endif
