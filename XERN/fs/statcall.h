#ifndef STATCALL_H  // There is a file in the include folder name 'stat.h' that is why this file is named this way inorder to avoid confusion 
#define STATCALL_H

int kstat(char *path, struct stat *statbuf)
{	
	struct minode *fmip = search_inode(path);
	if(! fmip)
	{
		kprintf("Error: [stat(%s): No such file or directory]]\n", path);
		return -1;
	}
	
	statbuf->st_dev   = -1;  // Not used in our system
	statbuf->st_ino   = fmip->ino;
	statbuf->st_mode  = fmip->inode.i_mode;
	statbuf->st_nlink = fmip->inode.i_links_count;
	statbuf->st_uid   = fmip->inode.i_uid;
	statbuf->st_gid   = fmip->inode.i_gid;

	if(S_ISCHR(fmip->inode.i_mode) || S_ISBLK(fmip->inode.i_mode))
		statbuf->st_rdev = (unsigned short)fmip->inode.i_block[0];
	else
		statbuf->st_rdev = 0;

	statbuf->st_size    = fmip->inode.i_size; 
	statbuf->st_blksize = BLOCKSIZE;
	statbuf->st_blocks  = fmip->inode.i_blocks;
	statbuf->st_atim    = fmip->inode.i_atime;
	statbuf->st_mtim    = fmip->inode.i_mtime;
	statbuf->st_ctim    = fmip->inode.i_ctime;
	
	return 0; 
}

#endif 
