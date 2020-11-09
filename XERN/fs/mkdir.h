#ifndef MKDIR_H
#define MKDIR_H

extern char buf[BLOCKSIZE];

int kmkdir(char *path)
{

	char bname[strlen(path) + 1], dname[strlen(path) + 1];
	
	basename(path, bname);
	dirname(path, dname);

    struct minode *ipath = search_inode(path);
    if(ipath)
	{
		kprintf("Error: [mkdir(%s): File already exist]\n", path);
		return -1;
	}
	struct minode *parent = search_inode(dname);
	if(! parent)
	{
		kprintf("Error: [mkdir: %s: No such directory]\n", dname);
		return -1;
	}	
	if(! S_ISDIR(parent->inode.i_mode))
	{
		kprintf("Error: [mkdir: %s: Not a directory]\n", dname);
		return -1;
	}

	int new_inode = alloc_inode();

	if(! new_inode)
	{
		kprintf("Error: [mkdir(%s): Disk out of inodes]\n", path);
		return -1;
	}

	int new_block = alloc_block();

	if(! new_block)
	{
		kprintf("Error: [mkdir(%s): Disk out of block]\n", path);
		return -1;
	}
	if(! insert_child(parent, new_inode, bname))
	{
		kprintf("Error: [mkdir(%s): Possibly block alloc problem]\n", path);          // vdisk out of blocks or shortage of block for directory
		return -1;
	} 


	int blk = ((new_inode - 1) / INODES_PER_BLOCK) + inode_start;
	int ino_off = (new_inode - 1) % INODES_PER_BLOCK;

	getblock(blk, buf);
	struct ext2_inode *inode = (struct ext2_inode *)buf + ino_off;

	inode->i_mode = 040755;
	inode->i_uid = curr_proc->uid;
	inode->i_gid = curr_proc->gid;
	inode->i_size = BLOCKSIZE;

	inode->i_links_count = 2;
	inode->i_atime = inode->i_ctime = inode->i_mtime = 0;
	inode->i_blocks = SECS_PER_BLK;


	for(int i = 0; i < 15; i++)
		inode->i_block[i] = 0;
	inode->i_block[0] = new_block;


	//create a copy in a memory inode
	for(int i =0; i < NMINODES; i++)
	{
		struct minode *mip = &minodes[i];
		if(mip->refcount == 0)
		{
			mip->refcount = 1;
			mip->inode = *inode;
			mip->ino = new_inode;
			break;
		}
	}

	putblock(blk, buf);        //save created inode to disk

	/* '.' and '..' directory */

	memset(buf, 0, BLOCKSIZE);
	struct ext2_dir_entry_2 *dir_entry = (struct ext2_dir_entry_2 *)buf;

    /* '.' directory */
	dir_entry->inode = new_inode;
	dir_entry->rec_len = 12;
	dir_entry->name_len = 1;
	dir_entry->file_type = 2;
	strcpy(dir_entry->name, ".");


    /* '..' directory */
	dir_entry = (char *)dir_entry + dir_entry->rec_len;
	dir_entry->inode = parent->ino;
	dir_entry->rec_len = BLOCKSIZE - 12;
	dir_entry->name_len = 2;
	dir_entry->file_type = 2;
	strcpy(dir_entry->name, "..");

	putblock(new_block, buf);
	

	/* increasing parent's link count and rewriting it to disk. 
       remember that the i_size of parent might have change. check insert_child() '1st condition check'.
    */ 

	parent->inode.i_links_count += 1;

	blk = ((parent->ino - 1) / INODES_PER_BLOCK) + inode_start;
	ino_off = (parent->ino - 1) % INODES_PER_BLOCK;

	getblock(blk, buf);
	memcpy((char *)((struct ext2_inode *)buf + ino_off), &parent->inode, sizeof(struct ext2_inode));
	putblock(blk, buf);

	return 0;
}

#endif