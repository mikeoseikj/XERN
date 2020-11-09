#ifndef READ_H
#define READ_H

int kread(int fd, char *buf, int count)
{
	
	int device, major;

	if(fd < 0 || fd >= PROC_FD_MAX)
		return -1;

	struct oft *oft_ptr = curr_proc->fd[fd];
	if(! oft_ptr)
	{
		kprintf("read error\n");
		return -1;
	}

	if(oft_ptr->mode == READ_PIPE)
		return read_pipe(fd, buf, count);

	if(oft_ptr->mode == WRITE_PIPE)
		goto err;
    
    if(oft_ptr->m_ptr->refcount == 0) //Possibly: File has been unlinked 
    	return -1;

	if((oft_ptr->mode & O_DIRECTORY) == O_DIRECTORY)
		return -1;

	if((oft_ptr->mode & 1) != O_RDONLY)	 
	{
		if((oft_ptr->mode & O_RDWR) != O_RDWR)
		{
			err:
				kprintf("Error: [read(%d): File not opened in read mode]\n", fd);
				return -1;
		}
	}
	

	int mode = oft_ptr->m_ptr->inode.i_mode;

	/* If file is a character device (console or serial IO) */
	if(S_ISCHR(mode))
	{
		device = oft_ptr->m_ptr->inode.i_block[0];
		major = (device & 0xFF00) >> 8;

		if(major < 0 || major >= NDEV || !devsw[major].read)
			return -1;

		return (*devsw[major].read)(buf, count);		// Console write
	}

	if(S_ISREG(mode))
		return read_file(fd, buf, count);

    return -1;
	

}



#endif