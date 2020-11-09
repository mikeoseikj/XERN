#ifndef FILE_H
#define FILE_H

char fbuf[BLOCKSIZE];
extern int buf12[BLOCKSIZE/4], buf13[BLOCKSIZE/4], dbuf[BLOCKSIZE/4];


int read_file(int fd, char *buf, int count)
{
    int blk, lbk, start;
	int n = count;
	int offset = curr_proc->fd[fd]->offset;

	struct minode *fmip = curr_proc->fd[fd]->m_ptr;

	int size = fmip->inode.i_size - offset; //bytes available in file
    int nread = 0;
	while(n > 0 && size > 0)	// 'n' is a copy of the 'count' parameter
	{
		lbk = offset / BLOCKSIZE;
		start = offset % BLOCKSIZE;

		if(lbk < 12)
		{
			blk = fmip->inode.i_block[lbk];
		} 
		else if(lbk >= 12 && lbk < ((BLOCKSIZE/4) + 12))   /* Indirect blocks */
		{
			getblock(fmip->inode.i_block[12], (char *)buf12);
			blk = buf12[lbk - 12];
		}
		else      /* Double indirect blocks */
		{
			lbk -= (12 + (BLOCKSIZE/4));
			getblock(fmip->inode.i_block[13], (char *)buf13);
			getblock(buf13[lbk / (BLOCKSIZE/4)], (char *)dbuf);
			blk = dbuf[lbk % (BLOCKSIZE/4)];
		}

		getblock(blk, fbuf);
		
	    int remain = BLOCKSIZE - start;	    
	    int len = size > n? n : size;
	    len = len > remain? remain : len;

        memcpy(buf + nread, fbuf + start, len);

	    nread += len; offset += len;
	    size -= len; n -= len;
	   
	   
	}
	curr_proc->fd[fd]->offset = offset;
	buf[nread] = 0;
	return nread;
}


int write_file(int fd, char *buf, int count)
{
    int lbk, start;
    int blk, bk12, bk13, dbk;
	int n = count;
	int offset = curr_proc->fd[fd]->offset;

	struct minode *fmip = curr_proc->fd[fd]->m_ptr;

	int size = strlen(buf);

    int nwrite = 0;
	while(n > 0 && size > 0)	// Note: 'n' is a copy of the 'count' parameter
	{
		lbk = offset / BLOCKSIZE;
		start = offset % BLOCKSIZE;

		if(lbk < 12)	/* Direct blocks */
		{
			blk = fmip->inode.i_block[lbk];
			if(blk == 0)
			{
				blk = fmip->inode.i_block[lbk] = alloc_block();
				if(blk == 0)
				{
					kprintf("No more data blocks\n");
					return -1;
				}
				fmip->inode.i_blocks += SECS_PER_BLK;
			}

		} 
		else if(lbk >= 12 && lbk < ((BLOCKSIZE/4) + 12))   /* Indirect blocks */
		{
			lbk -= 12;
			if(fmip->inode.i_block[12] == 0)
			{
				bk12 = fmip->inode.i_block[12] = alloc_block();

				if(bk12 == 0)
				{
					kprintf("No more data blocks\n");
				    return -1;
				}
				for(int i = 0; i < (BLOCKSIZE/4); i++)
					buf12[i] = 0;
				putblock(fmip->inode.i_block[12], (char *)buf12);
			}

			getblock(fmip->inode.i_block[12], (char *)buf12);
			blk = buf12[lbk];

			if(blk == 0)
			{
				blk = buf12[lbk] = alloc_block();

				if(blk == 0)
				{
					kprintf("No more data blocks\n");
					return -1;
				}
				fmip->inode.i_blocks += SECS_PER_BLK;
				putblock(fmip->inode.i_block[12], (char *)buf12);
			}
			
		}
		else      /* double indirect blocks */
		{
			lbk -= (12 + (BLOCKSIZE/4));

			if(fmip->inode.i_block[13] == 0)
			{
				bk13 = fmip->inode.i_block[13] = alloc_block();

				if(bk13 == 0)
				{
					kprintf("No more data blocks\n");
					return -1;
				}
				for(int i = 0; i < (BLOCKSIZE/4); i++)
					buf13[i] = 0;
				putblock(fmip->inode.i_block[13], (char *)buf13);
			}
			getblock(fmip->inode.i_block[13], (char *)buf13);

			dbk = buf13[lbk/(BLOCKSIZE/4)];

			if(dbk == 0)
			{
				dbk = buf13[lbk/(BLOCKSIZE/4)] = alloc_block();

				if(dbk == 0)
				{
					kprintf("No more data blocks\n");
					return -1;
				}
				for(int i =0 ; i < (BLOCKSIZE/4); i++)
					dbuf[i] = 0;

				putblock(fmip->inode.i_block[13], (char *)buf13);
				putblock(dbk, (char *)dbuf);
			}
			 
			getblock(dbk , (char *)dbuf);
			blk = dbuf[lbk % (BLOCKSIZE/4)];
			
			if(blk == 0)
			{
				blk = dbuf[lbk % (BLOCKSIZE/4)] = alloc_block();
				if(blk == 0)
				{
					kprintf("No more data blocks\n");
					return -1;
				}
				fmip->inode.i_blocks += SECS_PER_BLK;
				putblock(dbk, (char *)dbuf);
			}
		}

		getblock(blk, fbuf);

		int remain = BLOCKSIZE - start;	    
	    int len = size > n? n : size;
	    len = len > remain? remain : len;

		memcpy(fbuf + start, buf + nwrite, len);      

        offset += len; nwrite += len;
        n -= len; size -= len;

		putblock(blk, fbuf);
	}

	fmip->inode.i_size = offset;
	curr_proc->fd[fd]->offset = offset;

	/* size of file might have changed and the allocation of new blocks */
	blk = ((fmip->ino -1) / INODES_PER_BLOCK) + inode_start;
	int ino_off = (fmip->ino - 1) % INODES_PER_BLOCK;

	getblock(blk, fbuf);
    memcpy((char *)((struct ext2_inode *)fbuf + ino_off), &fmip->inode, sizeof(struct ext2_inode));
	putblock(blk, fbuf);
	
	return nwrite;

}


#endif