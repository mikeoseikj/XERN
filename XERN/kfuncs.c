#include "include/globals.h"
#include "include/process.h"

int kgetpid()
{
	return curr_proc->pid;
}

int kgetppid()
{
	return curr_proc->ppid;
}

int kgetuid()
{
	return curr_proc->uid;
}

int kgetgid()
{
	return curr_proc->gid;
}

int ksetuid(int uid)
{
	if(uid < 0)
		return -1;

	curr_proc->uid = uid;
	return 0;
}

int ksetgid(int gid)
{
	if(gid < 0)
		return -1;

	curr_proc->gid = gid;
	return 0;
}

unsigned int slimit;
void ksleep(unsigned int seconds)
{
	extern struct __TIMER *p_time0;
	slimit = p_time0->seconds + seconds;
	while(p_time0->seconds < slimit);
}

int kreadpass(char *buf, int count) 
{
	char c, *bptr = buf;
	int nread = 0;  // bytes read
	while(1)
	{
		c = uart_getc();
		
		if(c == 0x7F)  // backspace
		{
			if(nread > 0)
				bptr--, nread--;

			continue;
		}
		if(c == '\r')
		{
			*bptr = 0;
			uart_putc('\n');
			uart_putc('\r');
			return nread;
		}

		*bptr++ = c, nread++;	
		if(nread == count)
		{
			*bptr = 0;
			uart_putc('\n');
			uart_putc('\r');
			return count;
		}
	}
}


int kget_procs(struct process *processes[])
{
	struct proc *p;
	int count = 0;
	for(int i = 1; i < NPROCS; i++)
	{
		p = &procs[i];
		if(p->state != ZOMBIE && p->state != UNUSED)
		{
			strcpy(processes[count]->name, p->name);
			processes[count]->pid = p->pid;
			if(p->pid != curr_proc->pid)
				processes[count]->state = 0;
			else
				processes[count]->state = 1;
			processes[count]->size = p->size;
			count++;
		}
	}	
	return count;
}

unsigned int kget_boottime()
{
	extern struct __TIMER *p_time0;
	return (unsigned int)p_time0->seconds;
}

int ksetlogin(char *username)
{
	if(strlen(username) == 0)
		return -1;

	strncpy(curr_proc->owner_name, username, 64);
	return 0;
}

void kgetlogin(char buf[])
{
	strcpy(buf, curr_proc->owner_name);
}
