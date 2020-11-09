#ifndef OPEN_H
#define OPEN_H

#define OWNER  000700
#define GROUP  000070
#define OTHER  000007

int check_access_permission(struct minode *mip, int flags)
{
	int permbits;
	if(curr_proc->uid == mip->inode.i_uid)
		permbits = (mip->inode.i_mode & OWNER) >> 6;
	else if(curr_proc->gid == mip->inode.i_gid)
		permbits = (mip->inode.i_mode & GROUP) >> 3;
	else 
		permbits = mip->inode.i_mode & OTHER; 

	if((flags & 1) == O_RDONLY && (permbits & 4) == 0)
		return 0;

	if((flags & O_WRONLY) == O_WRONLY && (permbits & 2) == 0)
		return 0;
      
	if((flags & O_RDWR) == O_RDWR && (permbits & 4) == 0)
		return 0;
     
	if((flags & O_RDWR) == O_RDWR && (permbits & 2) == 0)
		return 0;

	return 1;      //true
}


int kopen(char *file, int flags)
{
	struct minode *fmip = search_inode(file);

	if(! fmip)
	{
		if(((flags & (O_CREAT|O_WRONLY)) == (O_CREAT|O_WRONLY)) || ((flags & (O_CREAT|O_RDWR)) == (O_CREAT|O_RDWR)))
		{
			if(kcreat(file) < 0)
				return -1;
			fmip = search_inode(file);
		}
		else
		{
			kprintf("Error: [open(%s): File does not exist]\n", file);
			return -1;
		}
	}

	if((flags & O_DIRECTORY) == O_DIRECTORY)
	{
		if(!S_ISDIR(fmip->inode.i_mode))
		{
			kprintf("Error: [open(%s): Not a directory]\n", file);
			return -1;
		}
	}


    /* check access permission if i am  not a root user (trivial to check for it root users becasue root can access any file regardless of permissions) */
	if(curr_proc->uid != 0)
	{
		if(! check_access_permission(fmip, flags))
		{
			kprintf("Error: [open(%s): Access permission denied]\n", file);
			return -1;
		}
	}


   /* If a file is already opened in WRITE/APPEND/TRUNC mode, we should only be allowed to
     read the file and be restricted from either modes in the the current program being executed
   */

	if(!S_ISCHR(fmip->inode.i_mode) && !S_ISBLK(fmip->inode.i_mode))
	{
		for(int i = 0; i < PROC_FD_MAX; i++)
		{
			/* We don't have to compare by '->ino' becuase get_inode() does that which is in the end, called by search_inode() */
			if(curr_proc->fd[i]->m_ptr == fmip)
			{
				if((flags != O_RDONLY) && (curr_proc->fd[i]->mode != O_RDONLY) && ((flags & O_DIRECTORY) != O_DIRECTORY))
				{
					kprintf("Error: [open(%s)]: Already opened in modification mode]\n", file);
					return -1;
				}
			}
		}
	}

	struct oft *oft_ptr = oft_alloc();
	if(! oft_ptr)
	{
		oft_dalloc(oft_ptr);
		kprintf("Error: [System out of OFTs]\n");
		return -1;
	}
	oft_ptr->mode = flags;
	oft_ptr->m_ptr = fmip;
    oft_ptr->offset = 0;

	if(((flags & O_WRONLY) == O_WRONLY) || ((flags & O_RDWR) == O_RDWR))
	{
		if(S_ISREG(fmip->inode.i_mode))
		{
			if((flags & O_APPEND) == O_APPEND)
				oft_ptr->offset = fmip->inode.i_size;

			if((flags & O_TRUNC) == O_TRUNC)
			{
				free_iblocks(fmip);
				oft_ptr->offset = 0;
			}
		}
	}

	/* Check if process's fd MAX is reached */
	for(int file_desc = 0; file_desc < PROC_FD_MAX; file_desc++)
	{
		if(curr_proc->fd[file_desc] == 0)
		{
			curr_proc->fd[file_desc] = oft_ptr;
			return file_desc;	// if function was successful
		}
	}
	
	kprintf("Error: [Process MAX FD reached]\n");
	oft_dalloc(oft_ptr);
	return -1;

}


#endif