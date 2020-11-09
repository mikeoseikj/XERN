#ifndef CREAT_H 
#define CREAT_H

extern char buf[BLOCKSIZE];

int kcreat(char *path)
{
	char bname[strlen(path) + 1], dname[strlen(path) + 1];

	basename(path, bname);
	dirname(path, dname);

	struct minode *ipath = search_inode(path);
    if(ipath)
	{
		kprintf("Error: [creat(%s): File already exist]\n", path);
		return -1;
	}
	struct minode *parent = search_inode(dname);
	if(! parent)
	{
		kprintf("Error: [creat: %s: No such directory]\n", dname);
		return -1;
	}	

	if(! S_ISDIR(parent->inode.i_mode))
	{
		kprintf("Error: [creat: %s: Not a directory]\n", dname);
		return -1;
	}

	int new_inode = alloc_inode();
	if(! new_inode)
	{
		kprintf("Error: [creat(%s): Virtual disk out of inodes]\n", path);
		return -1;
	}
	if(! insert_child(parent, new_inode, bname))
	{
		kprintf("Error: [creat(%s): block alloc problem]\n", path);          // vdisk out of blocks or shortage of block for directory
		return -1;
	}   

	int blk = ((new_inode - 1) / INODES_PER_BLOCK) + inode_start;
	int ino_off = (new_inode - 1) % INODES_PER_BLOCK;

	getblock(blk, buf);
	struct ext2_inode *inode = (struct ext2_inode *)buf + ino_off;

	inode->i_mode = 0100644;
	inode->i_uid = curr_proc->uid;
	inode->i_gid = curr_proc->gid;
	inode->i_size = 0;

	inode->i_links_count = 1;
	inode->i_atime = inode->i_ctime = inode->i_mtime = 0;
	inode->i_blocks = 0;

	
	for(int i = 0; i < 15; i++)
		inode->i_block[i] = 0;

	/* create a copy in a memory inode */
	for(int i =0; i < NMINODES; i++)
	{
		struct minode *mip = &minodes[i];
		if(mip->refcount == 0)
		{
			mip->refcount = 1;
			mip->ino = new_inode;
			mip->inode = *inode;
			break;
		}
	}

	putblock(blk, buf);        //save created inode to disk

	/* NOTE: parent i_links_count is not incremented! 
       remember that the i_size of parent might have change. check insert_child() '1st condition check'.
    */ 

	blk = ((parent->ino - 1) / INODES_PER_BLOCK) + inode_start;
	ino_off = (parent->ino - 1) % INODES_PER_BLOCK;

	getblock(blk, buf);
	memcpy((char *)((struct ext2_inode *)buf + ino_off), &parent->inode, sizeof(struct ext2_inode));
	putblock(blk, buf);

	return 0;
}


#endif