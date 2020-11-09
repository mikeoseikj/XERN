#ifndef UTILS_H
#define UTILS_H


int MAX_INODES = 0, MAX_BLOCKS = 0;
int block_bitmap, inode_bitmap, inode_start;
char buf[BLOCKSIZE];
int buf12[BLOCKSIZE/4], buf13[BLOCKSIZE/4], dbuf[BLOCKSIZE/4];

struct minode root_inode;
extern struct proc *curr_proc;

int tstbit(char *buf, int bit)
{
    if(buf[bit/8] & (1 << (bit % 8)))
    	return 1;

    return 0;
}
void setbit(char *buf, int bit)
{
	buf[bit/8] |= (1 << (bit % 8));
}
void clrbit(char *buf, int bit)
{
  buf[bit/8] &= ~(1 << (bit % 8));
}

void dec_free_inodes()
{
	getblock(0, buf);
	struct ext2_super_block *superblock = (struct ext2_super_block *)(buf + HALF_BLOCKSIZE);
	superblock->s_free_inodes_count--;
	putblock(0, buf);

	getblock(1, buf);
	struct ext2_group_desc *group_desc = (struct ext2_group_desc *)buf;
	group_desc->bg_free_inodes_count--;
	putblock(1, buf);
}

void dec_free_blocks()
{
	getblock(0, buf);
	struct ext2_super_block *superblock = (struct ext2_super_block *)(buf + HALF_BLOCKSIZE);
	superblock->s_free_blocks_count--;
	putblock(0, buf);

	getblock(1, buf);
	struct ext2_group_desc *group_desc = (struct ext2_group_desc *)buf;
	group_desc->bg_free_blocks_count--;
	putblock(1, buf);
}

int alloc_inode()
{
	getblock(inode_bitmap, buf);

	for(int i = 0; i < MAX_INODES; i++)
	{
		if(tstbit(buf, i) == 0)
		{
			setbit(buf, i);
			putblock(inode_bitmap, buf);

			dec_free_inodes();
			return (i + 1);
		}
	}
	return 0;
}

int alloc_block()
{
	getblock(block_bitmap, buf);
 
    /* block zero won't be available under normal circumstances on a valid filesystem */
    for(int i = 0; i < MAX_BLOCKS; i++)
	{
		if(tstbit(buf, i) == 0)
		{
			setbit(buf, i);
			putblock(block_bitmap, buf);

			dec_free_blocks();
			return i; 
		}
	}
	return 0;
}

void inc_free_inodes()
{
	getblock(0, buf);
	struct ext2_super_block *superblock = (struct ext2_super_block *)(buf + HALF_BLOCKSIZE);
	superblock->s_free_inodes_count++;
	putblock(0, buf);

	getblock(1, buf);
	struct ext2_group_desc *group_desc = (struct ext2_group_desc *)buf;
	group_desc->bg_free_inodes_count++;
	putblock(1, buf);
}

void inc_free_blocks()
{
	getblock(0, buf);
	struct ext2_super_block *superblock = (struct ext2_super_block *)(buf + HALF_BLOCKSIZE);
	superblock->s_free_blocks_count++;
	putblock(0, buf);

	getblock(1, buf);
	struct ext2_group_desc *group_desc = (struct ext2_group_desc *)buf;
	group_desc->bg_free_blocks_count++;
	putblock(1, buf);
}

void dalloc_inode(unsigned int ino)
{
	if(ino > MAX_INODES)
		return;

	getblock(inode_bitmap, buf);
	clrbit(buf, ino - 1);
	putblock(inode_bitmap, buf);
	inc_free_inodes();
	
}

void dalloc_block(unsigned int blk)
{
	if(blk >= MAX_BLOCKS)
		return;

	getblock(block_bitmap, buf);
	clrbit(buf, blk);
	putblock(block_bitmap, buf);
	inc_free_blocks();
}

struct oft *oft_alloc()
{
	for(int i = 0; i < NOFTS; i++)
	{
		if(open_file_table[i].refcount == 0)
		{
			open_file_table[i].refcount = 1;
			return &open_file_table[i];
		}
	}
	return 0;
}
void oft_dalloc(struct oft *ptr)
{
	ptr->m_ptr = 0;
	ptr->p_ptr = 0;
}



int brk_path(char *pathname, char dirnames[256][256])
{
	int dir_count = 0;
	char sto[strlen(pathname) + 1]; char *path = (char *)sto;
	memset(path, 0, strlen(pathname));
	int pos = 0;

	remove_multiple_slashes(pathname, path);

	if(path[0] == '/')
	{
		strcpy(dirnames[0], "/");
		path += 1, pos++;
		dir_count++;
	}

	for(int i = 0; i < strlen(path); i++)
	{
		if(path[i] == '/')
		{
			path[i] = 0;
			strncpy(dirnames[pos], path, 255);
			pos++, path += (i + 1), i = 0;

			dir_count++;
		}

		if(pos == 255) break;
	}
	if(strlen(path)) 
	{
        dir_count++;
		strncpy(dirnames[pos], path, 255);
	}

	return dir_count;
}


struct minode *get_inode(int ino)
{
	struct minode *mptr;
	for(int i = 0; i < NMINODES; i++)
	{
		mptr = &minodes[i];
		if(mptr->refcount && (mptr->ino == ino))
			return mptr;
	}

	int blk = ((ino - 1) / INODES_PER_BLOCK) + inode_start;
	int ino_off = (ino - 1) % INODES_PER_BLOCK;

	getblock(blk, buf);
	struct ext2_inode *inode = (struct ext2_inode *)buf + ino_off;

	for(int i = 0; i < NMINODES; i++)
	{
		mptr = &minodes[i];

		if(mptr->refcount == 0)
		{
			mptr->refcount = 1;
			mptr->ino = ino;
			mptr->inode = *inode;
			return mptr; 
		}
	}

	return 0;

}

void FS_init()
{
	
    getblock(0, buf);
	struct ext2_super_block *superblock = (struct ext2_super_block *)(buf + HALF_BLOCKSIZE);

    MAX_INODES = superblock->s_inodes_count;
    MAX_BLOCKS = superblock->s_blocks_count;

	getblock((superblock->s_first_data_block + 1), buf);  // this is the same as 'getblock(1, buf)' since blocksize is 2KiB
	struct ext2_group_desc *group_desc = (struct ext2_group_desc *)buf;

    //assigning values for use later on
	block_bitmap = group_desc->bg_block_bitmap;
	inode_bitmap = group_desc->bg_inode_bitmap;
	inode_start = group_desc->bg_inode_table;

	for(int i =0; i < NMINODES; i++)
	{
		minodes[i].ino = 0;
		minodes[i].refcount = 0;
	}
    
	root_inode = *((struct minode *)get_inode(2));
   
}



struct minode *find_inode(char *dirname, struct minode *parent_inode)
{

	if(! S_ISDIR(parent_inode->inode.i_mode))
		return 0;
	
	int len;
	struct ext2_dir_entry_2 *dir_entry; 

	for(int i = 0; i < 12; i++)
	{
		if(parent_inode->inode.i_block[i] == 0)
			return 0;
		
        getblock(parent_inode->inode.i_block[i], buf);
		dir_entry = (struct ext2_dir_entry_2 *)buf;

		while(dir_entry < buf + BLOCKSIZE)
		{
			len = (strlen(dirname) > dir_entry->name_len)? strlen(dirname) : dir_entry->name_len; 		
	 		if(memcmp(dir_entry->name, dirname, len) == 0)
	 			return get_inode(dir_entry->inode);

			dir_entry = (char *)dir_entry + dir_entry->rec_len;
		}
	}

	
	if(parent_inode->inode.i_block[12])
	{
		getblock(parent_inode->inode.i_block[12], (char *)buf12);
        for(int i = 0; i < (BLOCKSIZE/4); i++)
        {
	    	if(buf12[i])
	    	{
	    		getblock(buf12[i], buf);
		        dir_entry = (struct ext2_dir_entry_2 *)buf;

				while(dir_entry < buf + BLOCKSIZE)
				{
					len = (strlen(dirname) > dir_entry->name_len)? strlen(dirname) : dir_entry->name_len;	 		
	 				if(memcmp(dir_entry->name, dirname, len) == 0)
	 					return get_inode(dir_entry->inode);

					dir_entry = (char *)dir_entry + dir_entry->rec_len;
				}

	    	}
	    	else
	    	{
	    		return 0;
	    	}
         
	    }
	}


	if(parent_inode->inode.i_block[13])
	{
        getblock(parent_inode->inode.i_block[13], (char *)buf13);
        for(int i = 0; i < (BLOCKSIZE/4); i++)
        {
	 		if(buf13[i])
	 		{
	   			getblock(buf13[i], (char *)dbuf);
           		for(int j = 0; j < (BLOCKSIZE/4); j++)
           		{
               		if(dbuf[j])
               		{
               			getblock(dbuf[j], buf);
		        		dir_entry = (struct ext2_dir_entry_2 *)buf;

						while(dir_entry < buf + BLOCKSIZE)
						{
							len = (strlen(dirname) > dir_entry->name_len)? strlen(dirname) : dir_entry->name_len;
	 		
	 						if(memcmp(dir_entry->name, dirname, len) == 0)
	 							return get_inode(dir_entry->inode);

							dir_entry = (char *)dir_entry + dir_entry->rec_len;
						}

               		}
               		else
               		{
               			return 0;
               		}
                  
           		}  
        	}
       	}
   	}
	
	return 0;
}

char dirnames[256][256]; 
struct minode *search_inode(char *path)
{ 
	char dname[256];
	int dir_count = brk_path(path, dirnames);

	strcpy(dname, dirnames[0]);
    struct minode *start_inode = (strcmp(dname ,"/") == 0)? &root_inode : curr_proc->cwd;
    int dir_off = (strcmp(dname,"/") == 0)? 1 : 0;

    for(int i = dir_off; i < dir_count; i++)
    {
    	strcpy(dname, dirnames[i]);
    	if(strcmp(dname, "root") == 0)                // make 'root'  the root directory
    		start_inode = &root_inode;
    	else
   	 		start_inode = find_inode(dname, start_inode);

    	if(! start_inode)
    		return 0;
    }
    return start_inode;
}


void free_iblocks(struct minode *mip)
{

	/* Deleting direct blocks */ 
	for(int i = 0; i < 12; i++)
	{
		if(mip->inode.i_block[i] == 0)
			break;

		dalloc_block(mip->inode.i_block[i]);
		mip->inode.i_block[i] = 0;
	}

	/* Deleting indirect blocks */ 
	if(mip->inode.i_block[12])
	{
		getblock(mip->inode.i_block[12], (char *)buf12);
		dalloc_block(mip->inode.i_block[12]);
		mip->inode.i_block[12] = 0;

		for(int i = 0; i < (BLOCKSIZE/4); i++)
		{
			if(buf12[i] == 0)
				break;

			dalloc_block(buf12[i]);
		}
		
	}

	/* Deleting double indirect blocks */
	if(mip->inode.i_block[13])
	{
		getblock(mip->inode.i_block[13], (char *)buf13);
		dalloc_block(mip->inode.i_block[13]);
		mip->inode.i_block[13]  = 0;

		for(int i = 0; i < (BLOCKSIZE/4); i++)
		{
			if(buf13[i] == 0)
				break;

			getblock(buf13[i], (char *)dbuf);
			dalloc_block(buf13[i]);

			for(int j  = 0; j < (BLOCKSIZE/4); j++)
			{
			    if(dbuf[j] == 0)
			    	break;

			    dalloc_block(dbuf[j]);
			}
		}
	}

	mip->inode.i_size = 0;
	mip->inode.i_blocks = 0;
	mip->inode.i_atime = mip->inode.i_mtime = 0;

	/* Save modification to the disk */
	int blk = ((mip->ino - 1) / INODES_PER_BLOCK) + inode_start;
	int ino_off = (mip->ino - 1) % INODES_PER_BLOCK;

	getblock(blk, buf);
	memcpy((char *)((struct ext2_inode *)buf + ino_off), &mip->inode, sizeof(struct ext2_inode));
	putblock(blk, buf);

}


/* Get name of existing 'verify/child' inode in parent directory */
int getname(int parent_inode, int verify_inode, char name[])
{
	struct minode *parent = get_inode(parent_inode);
	struct ext2_dir_entry_2 *dir_entry;

	for(int i = 0; i < 12; i++)
	{
		if(parent->inode.i_block[i] == 0)
			return 0;

        getblock(parent->inode.i_block[i], buf);
		dir_entry = (struct ext2_dir_entry_2 *)buf;

		while(dir_entry < buf + BLOCKSIZE)
		{			
			if(dir_entry->inode == verify_inode)
			{
				memcpy(name, dir_entry->name, dir_entry->name_len);
				name[dir_entry->name_len] = 0;
				return 1;
			}
      
			dir_entry = (char *)dir_entry + dir_entry->rec_len;
		}
	}


	if(parent->inode.i_block[12])
	{
		getblock(parent->inode.i_block[12], (char *)buf12);
        for(int i = 0; i < (BLOCKSIZE/4); i++)
        {
	    	if(buf12[i])
	    	{
	    		getblock(buf12[i], buf);
		        dir_entry = (struct ext2_dir_entry_2 *)buf;

				while(dir_entry < buf + BLOCKSIZE)
				{
					if(dir_entry->inode == verify_inode)
					{
						memcpy(name, dir_entry->name, dir_entry->name_len);
						name[dir_entry->name_len] = 0;
						return 1;
					}
      
					dir_entry = (char *)dir_entry + dir_entry->rec_len;
				}

	    	}
	    	else
	    	{
	    		return 0;
	    	}
         
	    }
	}

	if(parent->inode.i_block[13])
	{
        getblock(parent->inode.i_block[13], (char *)buf13);
        for(int i = 0; i < (BLOCKSIZE/4); i++)
        {
	 		if(buf13[i])
	 		{
	   			getblock(buf13[i], (char *)dbuf);
           		for(int j = 0; j < (BLOCKSIZE/4); j++)
           		{
               		if(dbuf[j])
               		{
               			getblock(dbuf[j], buf);
		        		dir_entry = (struct ext2_dir_entry_2 *)buf;

						while(dir_entry < buf + BLOCKSIZE)
						{			
							if(dir_entry->inode == verify_inode)
							{
								memcpy(name, dir_entry->name, dir_entry->name_len);
								name[dir_entry->name_len] = 0;
								return 1;
							}
      
							dir_entry = (char *)dir_entry + dir_entry->rec_len;
						}
               		}
               		else
               		{
               			return 0;
               		}
                  
           		}  
        	}
       	}
   	}

	return 0;

}


#endif