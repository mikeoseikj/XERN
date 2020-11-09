#ifndef GETDENT_H
#define GETDENT_H

extern struct proc *curr_proc;
extern char buf[BLOCKSIZE];
extern int buf12[BLOCKSIZE/4], buf13[BLOCKSIZE/4], dbuf[BLOCKSIZE/4];

int kgetdent(int fd, struct dirent *dir_entry)
{	
	struct oft *doft = curr_proc->fd[fd];

	if(fd < 0 || fd >= PROC_FD_MAX)
		return -1;

	if(! doft) 
		return -1;

	if((doft->mode & O_DIRECTORY) != O_DIRECTORY)
	{
		kprintf("Error: [getdent(%d): Not a directory]\n", fd);
		return -1;
	}

	int offset = curr_proc->fd[fd]->offset;
	struct minode *dmip = curr_proc->fd[fd]->m_ptr;

    /* If there is no more entries in the directory */
    if(offset >= dmip->inode.i_size)
    {
    	curr_proc->fd[fd]->offset = 0;
    	return 0;  
    }
	
	int blk, lbk = offset / BLOCKSIZE;
	int dir_off = offset % BLOCKSIZE;

	if(lbk < 12)
	{
		blk = dmip->inode.i_block[lbk];
	} 
	else if(lbk >= 12 && lbk < ((BLOCKSIZE/4) + 12))   /* Indirect blocks */
	{
		getblock(dmip->inode.i_block[12], (char *)buf12);
		blk = buf12[lbk - 12];
	}
	else      /* Double indirect blocks */
	{
		lbk -= (12 + (BLOCKSIZE/4));
		getblock(dmip->inode.i_block[13], (char *)buf13);
		getblock(buf13[lbk / (BLOCKSIZE/4)], (char *)dbuf);
		blk = dbuf[lbk % (BLOCKSIZE/4)];
	}

	getblock(blk, buf);
	struct ext2_dir_entry_2 *ext2_entry = (struct ext2_dir_entry_2 *)(buf + dir_off);

    dir_entry->d_namelen = ext2_entry->name_len;
	strncpy((char *)dir_entry->d_name, (char *)ext2_entry->name, ext2_entry->name_len);

	curr_proc->fd[fd]->offset += ext2_entry->rec_len;  // update oft's to the next entry in the directory 
	
	return 1;  // SUCCESS
}


#endif