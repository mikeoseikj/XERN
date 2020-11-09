#ifndef MODENTRY_H
#define MODENTRY_H

extern char buf[BLOCKSIZE];
extern int buf12[BLOCKSIZE/4], buf13[BLOCKSIZE/4], dbuf[BLOCKSIZE/4];

/*  FUNCTIONS FOR ADDING AND REMOVING ENTRIES FROM A DIRECTORY */

int insert_child(struct minode *parent, int child, char *entry_name)
{
	char name[256];
	strncpy(name, entry_name, 255);

	int needed_len = 4 * ((8 + strlen(name) + 3) / 4);
    struct ext2_dir_entry_2 *last_entry, *dir_entry;
    int blk, ideal_len, remain;

	/* Traverse through the direct blocks */
	for(int i = 0; i < 12; i++)
	{
		if(parent->inode.i_block[i] == 0)
		{
			blk = alloc_block();
			if(! blk) return 0;

			parent->inode.i_block[i] = blk;
			parent->inode.i_size += BLOCKSIZE;

			memset(buf, 0, BLOCKSIZE);
			dir_entry = (struct ext2_dir_entry_2 *)buf;
			dir_entry->inode = child;
			dir_entry->name_len = strlen(name);
			dir_entry->rec_len = BLOCKSIZE;
			memcpy(dir_entry->name, name, strlen(name));

			putblock(parent->inode.i_block[i], buf);

			return 1;      //true
		}

		getblock(parent->inode.i_block[i], buf);

	    dir_entry = (struct ext2_dir_entry_2 *)buf;

		while(dir_entry < buf + BLOCKSIZE)
		{
			last_entry = dir_entry;
			dir_entry = (char *)dir_entry + dir_entry->rec_len;
		}

		ideal_len = 4 * ((8 + last_entry->name_len + 3) / 4);
		remain = last_entry->rec_len - ideal_len;

		if(needed_len <= remain)
		{
			last_entry->rec_len = ideal_len;
			last_entry = (char *)last_entry + last_entry->rec_len;

			last_entry->inode = child;
			last_entry->name_len = strlen(name);
			last_entry->rec_len = remain;
			memcpy(last_entry->name, name, strlen(name));

			putblock(parent->inode.i_block[i], buf);

			return 1;      //true
		}
	}


    /* Traverse the indirect blocks */
	if(parent->inode.i_block[12] == 0)
	{
		blk  = alloc_block();
		if(! blk) return 0;
		parent->inode.i_block[12] = blk;

		for(int i = 0; i < (BLOCKSIZE/4); i++)
			buf12[i]  = 0;

		putblock(blk, (char *)buf12);
	}
	getblock(parent->inode.i_block[12], (char *)buf12);

	for(int i  = 0; i < (BLOCKSIZE/4); i++)
	{
		if(buf12[i] == 0)
		{
			blk = alloc_block();
			if(! blk) return 0;

			buf12[i] = blk;
			parent->inode.i_size += BLOCKSIZE;

			memset(buf, 0, BLOCKSIZE);
			dir_entry = (struct ext2_dir_entry_2 *)buf;
			dir_entry->inode = child;
			dir_entry->name_len = strlen(name);
			dir_entry->rec_len = BLOCKSIZE;
			memcpy(dir_entry->name, name, strlen(name));

			putblock(buf12[i], buf);

			return 1;      //true
		}


		getblock(buf12[i], buf);
	    dir_entry = (struct ext2_dir_entry_2 *)buf;

		while(dir_entry < buf + BLOCKSIZE)
		{
			last_entry = dir_entry;
			dir_entry = (char *)dir_entry + dir_entry->rec_len;
		}

		ideal_len = 4 * ((8 + last_entry->name_len + 3) / 4);
		remain = last_entry->rec_len - ideal_len;

		if(needed_len <= remain)
		{
			last_entry->rec_len = ideal_len;
			last_entry = (char *)last_entry + last_entry->rec_len;

			last_entry->inode = child;
			last_entry->name_len = strlen(name);
			last_entry->rec_len = remain;
			memcpy(last_entry->name, name, strlen(name));

			putblock(buf12[i], buf);

			return 1;      //true
		}
	}


    /* Traverse the double indirect blocks */
    if(parent->inode.i_block[13])
    {
    	blk  = alloc_block();
		if(! blk) return 0;
		parent->inode.i_block[13] = blk;

		for(int i = 0; i < (BLOCKSIZE/4); i++)
			buf13[i]  = 0;

		putblock(blk, (char *)buf13);
    }
    getblock(parent->inode.i_block[13], (char *)buf13);

    for(int i = 0; i < (BLOCKSIZE/4); i++)
    {
    	if(buf13[i] == 0)
    	{
    		blk = alloc_block();
    		if(! blk) return 0;

    		buf13[i] = blk;

    		for(int j = 0; j < (BLOCKSIZE/4); j++)
    		dbuf[j] = 0;

    		putblock(blk, (char *)dbuf);
    	}

        getblock(buf13[i], (char *)dbuf);
    	for(int k  = 0; k < (BLOCKSIZE/4); k++)
		{
		
			if(dbuf[k] == 0)
			{
				blk = alloc_block();
				if(! blk) return 0;

				dbuf[k] = blk;
				parent->inode.i_size += BLOCKSIZE;

				memset(buf, 0, BLOCKSIZE);
				dir_entry = (struct ext2_dir_entry_2 *)buf;
				dir_entry->inode = child;
				dir_entry->name_len = strlen(name);
				dir_entry->rec_len = BLOCKSIZE;
				memcpy(dir_entry->name, name, strlen(name));

				putblock(dbuf[k], buf);

				return 1;      //true
			}


			getblock(dbuf[k], buf);
	    	dir_entry = (struct ext2_dir_entry_2 *)buf;

			while(dir_entry < buf + BLOCKSIZE)
			{
				last_entry = dir_entry;
				dir_entry = (char *)dir_entry + dir_entry->rec_len;
			}

			ideal_len = 4 * ((8 + last_entry->name_len + 3) / 4);
			remain = last_entry->rec_len - ideal_len;

			if(needed_len <= remain)
			{
				last_entry->rec_len = ideal_len;
				last_entry = (char *)last_entry + last_entry->rec_len;

				last_entry->inode = child;
				last_entry->name_len = strlen(name);
				last_entry->rec_len = remain;
				memcpy(last_entry->name, name, strlen(name));

				putblock(dbuf[k], buf);

			return 1;      //true
			}
		}
    }

	return 0;     //false   
}


int remove_child(struct minode *parent, char *entry_name)
{
	struct ext2_dir_entry_2 *dir_entry, *prev_entry, *last_entry;
	int len, del_len;

	/* Traversing the direct blocks to find and remove entry name */
	for(int i = 0; i < 12; i++)
	{
		if(parent->inode.i_block[i] == 0)
			return 0; 

		getblock(parent->inode.i_block[i], buf);
		dir_entry = (struct ext2_dir_entry_2 *)buf;
	
		while(dir_entry < buf + BLOCKSIZE)
		{

			len = (strlen(entry_name) > dir_entry->name_len)? strlen(entry_name) : dir_entry->name_len;
			if(memcmp(dir_entry->name, entry_name, len) == 0)
			{
				/* if child is the only entry in a block ( this condition will only happen in the last block) */
				if(dir_entry->rec_len == BLOCKSIZE)
				{
					dalloc_block(parent->inode.i_block[i]);
					parent->inode.i_block[i] = 0;
     				parent->inode.i_size -= BLOCKSIZE;
					return 1; 			
				}
        		/* if last in the parent directory  */
				if(((char *)dir_entry + dir_entry->rec_len) == (buf + BLOCKSIZE))
				{
					prev_entry->rec_len += dir_entry->rec_len;
					putblock(parent->inode.i_block[i], buf);
					return 1;  
				}
				else   // if entry is in the middle
				{
					/* get last entry */
					last_entry = (struct ext2_dir_entry_2 *)buf;
					while(last_entry < buf + BLOCKSIZE)
					{
						prev_entry = last_entry;
						last_entry= (char *)last_entry + last_entry->rec_len;
					}
					last_entry = prev_entry;
					del_len = dir_entry->rec_len;
					last_entry->rec_len += del_len;

            		len = (((int)buf + BLOCKSIZE) - (int)dir_entry - del_len);

					memmove((char *)dir_entry, (char *)dir_entry + del_len, len);
					putblock(parent->inode.i_block[i], buf);
			
					return 1;
				}
			}
            prev_entry = dir_entry;
			dir_entry = (char *)dir_entry + dir_entry->rec_len;
		}	        
        
	}


	/* Traversing indirect blocks to remove entry name */

	if(parent->inode.i_block[12] == 0)
		return 0;

	getblock(parent->inode.i_block[12], (char *)buf12);
	for(int i  = 0; i < (BLOCKSIZE/4); i++)
	{
		if(buf12[i] == 0)
			return 0;

		getblock(buf12[i], buf);
		dir_entry = (struct ext2_dir_entry_2 *)buf;

		while(dir_entry < buf + BLOCKSIZE)
		{
			len = (strlen(entry_name) > dir_entry->name_len)? strlen(entry_name) : dir_entry->name_len;
			if(memcmp(dir_entry->name, entry_name, len) == 0)
			{
				/* if child is the only entry in a block( this condition will only happen in the last block) */
				if(dir_entry->rec_len == BLOCKSIZE)
				{
					dalloc_block(buf12[i]);
					buf12[i] = 0;
					putblock(parent->inode.i_block[12], (char *)buf12);
     				parent->inode.i_size -= BLOCKSIZE;
					return 1; 			
				}
        		/* if last in the parent directory */
				if(((char *)dir_entry + dir_entry->rec_len) == (buf + BLOCKSIZE))
				{
					prev_entry->rec_len += dir_entry->rec_len;
					putblock(buf12[i], buf);
					return 1;  
				}
				else   // if entry is in the middle
				{
					/* get last entry */
					last_entry = (struct ext2_dir_entry_2 *)buf;
					while(last_entry < buf + BLOCKSIZE)
					{
						prev_entry = last_entry;
						last_entry= (char *)last_entry + last_entry->rec_len;
					}
					last_entry = prev_entry;
					del_len = dir_entry->rec_len;
					last_entry->rec_len += del_len;

            		len = (((int)buf + BLOCKSIZE) - (int)dir_entry - del_len);

					memmove((char *)dir_entry, (char *)dir_entry + del_len, len);
					putblock(buf12[i], buf);
			
					return 1;
				}
			}
            prev_entry = dir_entry;
			dir_entry = (char *)dir_entry + dir_entry->rec_len;
		}
	     
	}




	/* Traversing through the double indirect blocks */

	if(parent->inode.i_block[13] == 0)
		return 0;

	getblock(parent->inode.i_block[13], (char *)buf13);
	for(int i  = 0; i < (BLOCKSIZE/4); i++)
	{
		if(buf13[i] == 0)
			return 0;

		getblock(buf13[i], (char *)dbuf);

		for(int j = 0; j < (BLOCKSIZE/4); j++)
		{
			if(dbuf[j] == 0)
				return 0;

			getblock(dbuf[j], buf);
			dir_entry = (struct ext2_dir_entry_2 *)buf;

			while(dir_entry < buf + BLOCKSIZE)
			{
				len = (strlen(entry_name) > dir_entry->name_len)? strlen(entry_name) : dir_entry->name_len;
				if(memcmp(dir_entry->name, entry_name, len) == 0)
				{
					/* if child is the only entry in a block */
					if(dir_entry->rec_len == BLOCKSIZE)
					{
						dalloc_block(dbuf[j]);
						dbuf[j] = 0;
						putblock(buf13[i], (char *)dbuf);
     					parent->inode.i_size -= BLOCKSIZE;
						return 1; 			
					}
        			/* if last in the parent directory */
					else if(((char *)dir_entry + dir_entry->rec_len) == (buf + BLOCKSIZE))
					{
						prev_entry->rec_len += dir_entry->rec_len;
						putblock(dbuf[j], buf);
						return 1;
					}
					else   // if entry is in the middle
					{
						/* get last entry */
						last_entry = (struct ext2_dir_entry_2 *)buf;
						while(last_entry < buf + BLOCKSIZE)
						{
							prev_entry = last_entry;
							last_entry= (char *)last_entry + last_entry->rec_len;
						}
						last_entry = prev_entry;
						del_len = dir_entry->rec_len;
						last_entry->rec_len += del_len;

            			len = (((int)buf + BLOCKSIZE) - (int)dir_entry - del_len);

						memmove((char *)dir_entry, (char *)dir_entry + del_len, len);
						putblock(dbuf[j], buf);
			
						return 1;
					}
				}
            	prev_entry = dir_entry;
				dir_entry = (char *)dir_entry + dir_entry->rec_len;
			}    
	    }
	}

	return 0;   //false
}

#endif