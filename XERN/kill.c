unsigned int user_exit_addr = 0x80000004;
int kkill(int pid)
{
	if(pid == 1)
	{
		kputs("Warning: [Not allowed to terminate the init process]\n");
		return -1;
	}
	struct proc *p;

	for(int i = 2; i < NPROCS; i++)
	{
		p = &procs[i];
		if(p->pid == pid && p->state != ZOMBIE && p->state != UNUSED)
		{
			p->tf->lr = user_exit_addr;
			return 0;
		}
	}
	kprintf("Error: [kill %d: No such process]\n", pid);
	return -1;
}