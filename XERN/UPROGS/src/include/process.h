#ifndef  PROCESS_H
#define  PROCESS_H

struct process
{
	char name[256];
	int pid;
	int state;
	int size;
};
#endif