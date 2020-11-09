char *ksbrk(int increment)
{
	int pos = curr_proc->size/0x100000; 
    int off = (curr_proc->size%0x100000) / PAGE_SIZE;
    unsigned int start_addr = (0x80000000 + (pos * 0x100000)) + (PAGE_SIZE * off);  

	if(increment == 0)
		return (char *)start_addr;

	if(increment > 0)
	{
		if(! allocuvm(curr_proc, increment))
		{
			kprintf("Error: sbrk(%d): Insufficient memory\n", increment);
			return -1;
		}

		return start_addr;
	}

	/* If a negative increment value was provided */
	unsigned int delsize = increment * -1;
	if((delsize % PAGE_SIZE) != 0)		//Roundup to the closest page size if necessary
		delsize = delsize + (PAGE_SIZE - (delsize % PAGE_SIZE));

	unsigned int delstart = start_addr - delsize;
	if(delstart < curr_proc->brk_start)
	{
		kprintf("Error: sbrk(%d): Trying to free non-heap memory\n", increment);
		return -1;
	}

	clrpage(curr_proc, delstart, delsize);
	return start_addr;  // in this context, start_addr is the previous location

}