#ifndef CHDIR_H
#define CHDIR_H

int kchdir(char *path)
{
	struct minode *fmip = search_inode(path);

	if(! fmip)
	{
		kputs("Error: [chdir: No such directory]\n");
		return -1;
	}

	char buf[61] = {0};
	if(S_ISLNK(fmip->inode.i_mode))
	{
		int len = (fmip->inode.i_size > 60)? 60 : fmip->inode.i_size;
		memcpy(buf, (char *)&fmip->inode.i_block, len);
		buf[len] = 0;
		fmip = search_inode((char *)buf);

		if(! fmip)
		{
			kprintf("Error: [chdir: %s - No such directory]\n", buf);
			return -1;
		}
	}

	if(! S_ISDIR(fmip->inode.i_mode))
	{
		kputs("Error: [chdir: Not a directory]\n");
		return -1;
	}

	curr_proc->cwd = fmip;
	return 0;
}

#endif