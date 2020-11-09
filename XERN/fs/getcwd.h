#ifndef GETCWD_H
#define GETCWD_H

extern char buf[BLOCKSIZE];

char tmp_cwd[256*66]; // Enough space to hold path. It is unlikedly for a path to be up to even 10% of its size
char *kgetcwd(char *buffer, unsigned int size)
{	
	int p_inodes[64] = {0}, v_inodes[64] = {0};
	int index = 0;

	struct minode *parent = curr_proc->cwd;
	while(1)
	{	
		getblock(parent->inode.i_block[0], buf);
	    struct ext2_dir_entry_2 *dir_entry = (struct ext2_dir_entry_2 *)buf;    // First entry is the current directory
	    v_inodes[index] = dir_entry->inode;                  // Inode of the directory we want to find

	    dir_entry = (char *)dir_entry + dir_entry->rec_len;         // Points to the parent directory, which contain the inode 
	    p_inodes[index] = dir_entry->inode;

	    if(p_inodes[index] == root_inode.ino)
	    	break;

	    parent = get_inode(dir_entry->inode);
	    index = index + 1;

	    if(index == 64)
	    	break;

	}
	
	if(v_inodes[0] == p_inodes[0] )          // This condition occurs when you pwd in the root directory. get_name() will return '.'
	{
		if(size < 6) //+null
		{
			kprintf("Error: [getcwd: cwd path size problem]\n");
			return 0;
		}

	  	strcpy(buffer, "/root");
	  	return buffer;
	}
    
    char name[256];
    strcpy(tmp_cwd, "/root");
    int offset = 5;

	for(int i = index; i >= 0; i--)
	{
		if(getname(p_inodes[i], v_inodes[i], name) == 0)
			return 0;
		
		tmp_cwd[offset] = '/';
		offset += 1;
		strcpy(tmp_cwd + offset, name);
		offset += strlen(name);
	}

	if(size < strlen(tmp_cwd) + 1) //+null
	{
		kprintf("Error: [getcwd: cwd path size problem]\n");
		return 0;
	}

	strcpy(buffer, tmp_cwd);
	return buffer;
}

#endif