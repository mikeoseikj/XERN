#ifndef CHMOD_H
#define CHMOD_H

int kchmod(char *path, unsigned short mode)
{
	struct minode *fmip = search_inode(path);

	if(! fmip)
	{
		kputs("Error: [chmod: No such file or directory]\n");
		return -1;
	}

	if(curr_proc->uid != 0 && curr_proc->uid != fmip->inode.i_uid)
	{
		kputs("Error: [chmod: You don\'t have the rights to change access permission of the file\n");
		return -1;
	}

	fmip->inode.i_mode = ((fmip->inode.i_mode & 00177000) | (mode & 0777)); //first 7bits '+' 9 bits permission

	int blk = ((fmip->ino - 1) / INODES_PER_BLOCK) + inode_start;
	int ino_off = (fmip->ino - 1) % INODES_PER_BLOCK;

	getblock(blk, buf);
	memcpy((char *)((struct ext2_inode *)buf + ino_off), &fmip->inode, sizeof(struct ext2_inode));
	putblock(blk, buf);

	return 0;

}

#endif