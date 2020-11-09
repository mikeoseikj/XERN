#ifndef UNLINK_H
#define UNLINK_H

extern char buf[BLOCKSIZE];

int kunlink(char *path)
{
	int blk, ino_off;  
	char bname[strlen(path) + 1], dname[strlen(path) + 1];
	
	basename(path, bname);
    dirname(path, dname);

	struct minode *parent = search_inode(dname);
	struct minode *child = search_inode(path);

	if(! child)
	{
		kprintf("Error: unlink(%s): No such file or directory]\n", path);
		return -1;
	}

	if(S_ISDIR(child->inode.i_mode))
	{
		kprintf("Error: [unlink(%s): Not permitted on a directory]\n", path);
		return -1;
	}
	 if(curr_proc->uid != 0 && curr_proc->uid != child->inode.i_uid)
    {
    	kprintf("Error: [unlink(%s): You don't have permission to delete this file]\n", path);
    	return -1;
    }

	if(! remove_child(parent, bname))
	{
		kprintf("Error: [unlink(%s): Possibly internal fs problem]\n", path);
    	return -1;
	}

	if(child->inode.i_links_count == 1)  // then we are going to delete the file
	{
		if(! S_ISLNK(child->inode.i_mode))   // unlink works on reg files and symlinks but symlinks has not data (allocated blocks)
			free_iblocks(child);
		dalloc_inode(child->ino);
	}
	else if(child->inode.i_links_count > 1)
	{
		child->inode.i_links_count--;

		blk = ((child->ino - 1) / INODES_PER_BLOCK) + inode_start;
	    ino_off = (child->ino - 1) % INODES_PER_BLOCK;

	   getblock(blk, buf);
	   memcpy((char *)((struct ext2_inode *)buf + ino_off), &child->inode, sizeof(struct ext2_inode));
	   putblock(blk, buf);
	}    

    /* NOTE: don't decrease parent i_links_count by one
       also the parent i_size might have been modified check the first condition check  in remove child
    */

	blk = ((parent->ino - 1) / INODES_PER_BLOCK) + inode_start;
	ino_off = (parent->ino - 1) % INODES_PER_BLOCK;

	getblock(blk, buf);
	memcpy((char *)((struct ext2_inode *)buf + ino_off), &parent->inode, sizeof(struct ext2_inode));
	putblock(blk, buf);

	/* remove it from memory even though there might be some links left for consistency */	
	for(int i = 0; i < NMINODES; i++)
	{
		struct minode *mip = &minodes[i];
		if(mip->refcount && mip->ino == child->ino)
		{
			mip->refcount = 0;
			break;
		}
	}

	return 0;
}


#endif