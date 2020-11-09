#ifndef LINK_H
#define LINK_H

extern char buf[BLOCKSIZE];

int klink(char *oldpath, char *newpath)
{
	char bname[strlen(newpath) + 1], dname[strlen(newpath) + 1];

	basename(newpath, bname);
	dirname(newpath, dname);

	struct minode *old = search_inode(oldpath);
	struct minode *new = search_inode(newpath);

	struct minode *parent = search_inode(dname);

	if(! old)
	{
		kprintf("Error: [link: %s: File does not exist]\n", oldpath);
		return -1;
	}
	if(! parent)
	{
		kprintf("Error: [link: %s: No such directory]\n", dname);
		return -1;
	}
	if(! S_ISDIR(parent->inode.i_mode))
	{
		kprintf("Error: [link: %s: Not a directory]\n", dname);
		return -1;
	}
	if(S_ISDIR(old->inode.i_mode))
	{
		kprintf("Error: [link(%s, %s): Not permitted on a directory]\n", oldpath, newpath);
		return -1;
	}
	if(new)           // passing this check doesn't neccessarily mean parent exist
	{
		kprintf("Error: [link(%s, %s): File already exists]\n", oldpath, newpath);
		return -1;
	}

    /* insert file into its parent directory */
	if(! insert_child(parent, old->ino, bname))
	{
		kprintf("Error: [link(%s, %s): block alloc problem]\n", oldpath, newpath);     // vdisk out of blocks or shortage of block for directory
		return -1;
	}  

	int blk = ((old->ino - 1) / INODES_PER_BLOCK) + inode_start;
	int ino_off = (old->ino - 1) % INODES_PER_BLOCK;

	getblock(blk, buf);

	old->inode.i_links_count++;
	memcpy((char *)((struct ext2_inode *)buf + ino_off), &old->inode, sizeof(struct ext2_inode));
	putblock(blk, buf);        //save modified inode to disk

	/* NOTE: parent i_links_count is not incremented! 
       remember that the i_size of parent might have change. check child_file() '1st condition check'.
    */

	blk = ((parent->ino - 1) / INODES_PER_BLOCK) + inode_start;
	ino_off = (parent->ino - 1) % INODES_PER_BLOCK;

	getblock(blk, buf);
	memcpy((char *)((struct ext2_inode *)buf + ino_off), &parent->inode, sizeof(struct ext2_inode));
	putblock(blk, buf);

	return 0;
}



#endif