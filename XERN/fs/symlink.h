
#ifndef SYMLINK_H
#define SYMLINK_H

extern char buf[BLOCKSIZE];

/* I implemented it to support only fast symbolic links for sake of simplicity */
int ksymlink(char *target, char *linkpath)
{
	char dname[strlen(linkpath) + 1], bname[strlen(linkpath) + 1];

	basename(linkpath, bname);
	dirname(linkpath, dname);

    struct minode *link = search_inode(linkpath);
	struct minode *parent = search_inode(dname);

    char clean_target_path[strlen(target) + 1];
    remove_multiple_slashes(target, clean_target_path);
    strncpy(clean_target_path, clean_target_path, 60);   //fast symbolic links can store a path of max length 60(characters)
    
	if(! parent)
	{
		kprintf("Error: [symlink: %s: No such directory]\n", dname);
		return -1;
	}
	if(! S_ISDIR(parent->inode.i_mode))
	{
		kprintf("Error: [symlink: %s: Not a directory]\n", dname);
		return -1;
	}
	if(link)    
	{
		kprintf("Error: [symlink: %s: File already exists]\n", linkpath);
		return -1;
	}
	int new_inode = alloc_inode();

	if(! new_inode)
	{
		kprintf("Error: [symlink(%s, %s): Virtual disk out of inodes]\n", target, linkpath);
		return -1;
	}


	if(! insert_child(parent, new_inode, bname))
	{
		kprintf("Error: [symlink:(%s, %s): block alloc problem]\n",target, linkpath);         
		return -1;
	}      
    


	int blk = ((new_inode - 1) / INODES_PER_BLOCK) + inode_start;
	int ino_off = (new_inode - 1) % INODES_PER_BLOCK;

	getblock(blk, buf);
	struct ext2_inode *inode = (struct ext2_inode *)buf + ino_off;

	inode->i_mode = 0xA1FF;
	inode->i_uid = 0;
	inode->i_gid = 0;
	inode->i_size = strlen(clean_target_path);
	inode->i_links_count = 1;
	inode->i_atime = inode->i_ctime = inode->i_mtime = 0;
	inode->i_blocks = 0;

	memcpy((char *)inode->i_block, clean_target_path, strlen(clean_target_path));

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
       remember that the i_size of parent might have change. check enter_child() '1st condition check'.
    */

	blk = ((parent->ino - 1) / INODES_PER_BLOCK) + inode_start;
	ino_off = (parent->ino - 1) % INODES_PER_BLOCK;

	getblock(blk, buf);
	memcpy((char *)((struct ext2_inode *)buf + ino_off), &parent->inode, sizeof(struct ext2_inode));
	putblock(blk, buf);

	return 0;
}


#endif