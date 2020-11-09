#ifndef RMDIR_H
#define RMDIR_H

extern char buf[BLOCKSIZE];

int krmdir(char *path)
{

	char bname[strlen(path) + 1], dname[strlen(path) + 1];
	basename(path, bname);
	dirname(path, dname);

    struct minode *parent = search_inode(dname);
    struct minode *child = search_inode(path);

	if(! child)
	{
		kprintf("Error: [rmdir(%s): No such file or directory]\n", path);
		return -1;
	}
	if(! S_ISDIR(child->inode.i_mode))
	{
		kprintf("Error: [rmdir(%s): Not a directory]\n", path);
		return -1;
	} 
    if((strcmp(bname, ".") == 0) || (strcmp(bname, "..") == 0) || (child->ino == curr_proc->cwd->ino)) // Note: you cant also remove current working directory
    {
    	kprintf("Error: [rmdir(%s): Operation not allowed]\n", path);
    	return -1;
    }
    if(curr_proc->uid != 0 && curr_proc->uid != child->inode.i_uid)
    {
    	kprintf("Error: [rmdir(%s): You don't have permission to delete this folder]\n", path);
    	return -1;
    }
   

    /* Checking if target folder is empty. 
       If empty, last dir entry inode must be equal parent inode (..)
    */

    struct ext2_dir_entry_2 *dir_entry, *last_entry;
    getblock(child->inode.i_block[0], buf);

    dir_entry = (struct ext2_dir_entry_2 *)buf;

    while(dir_entry < buf + BLOCKSIZE)
    {
    	last_entry = dir_entry;
    	dir_entry = (char *)dir_entry + dir_entry->rec_len;
    }
    if(last_entry->inode != parent->ino)
    {
    	kprintf("Error: [rmdir(%s): Directory not empty]\n", path);
    	return -1;
    }
   
	if(! remove_child(parent, bname))
	{
		kprintf("Error: [rmdir(%s): Possibly internal fs problem]\n", path);
    	return -1;
	}
	dalloc_inode(child->ino);

	/* free childs single data block which contains '.' and '..' */
    dalloc_block(child->inode.i_block[0]);

    /* NOTE: decrease parent i_links_count by one
       also the parent i_size might have been modified check the first condition check  in remove child
    */
    
	int blk = ((parent->ino - 1) / INODES_PER_BLOCK) + inode_start;
	int ino_off = (parent->ino - 1) % INODES_PER_BLOCK;

    parent->inode.i_links_count -= 1;

	getblock(blk, buf);
	memcpy((char *)((struct ext2_inode *)buf + ino_off), &parent->inode, sizeof(struct ext2_inode));
	putblock(blk, buf);
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