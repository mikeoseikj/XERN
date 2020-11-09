#ifndef CHOWN_H
#define CHOWN_H

int kchown(char *path, int uid, int gid)
{
	struct minode *fmip = search_inode(path);

	if(! fmip)
	{
		kputs("Error: [chown: No such file or directory]\n");
		return -1;
	}

	if(curr_proc->uid != 0 && curr_proc->uid != fmip->inode.i_uid)
	{
		kputs("Error: [chown: You don\'t have permission to change the owner of the file\n");
		return -1;
	}

	fmip->inode.i_uid = uid;
	fmip->inode.i_gid = gid;
	int blk = ((fmip->ino - 1) / INODES_PER_BLOCK) + inode_start;
	int ino_off = (fmip->ino - 1) % INODES_PER_BLOCK;

	getblock(blk, buf);
	memcpy((char *)((struct ext2_inode *)buf + ino_off), &fmip->inode, sizeof(struct ext2_inode));
	putblock(blk, buf);

	return 0;
}

#endif