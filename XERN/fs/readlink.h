#ifndef READLINK_H
#define READLINK_H


int kreadlink(char *path, char buf[])
{
	struct minode *minode = search_inode(path);
	if(! minode)
	{
		kprintf("Error: [readlink(%s): No such file or directory]\n",path);
		return -1; 
	}
	if(! S_ISLNK(minode->inode.i_mode))
	{
		kprintf("Error: [readlink(%s): Not a symbolic link]\n",path);
		return -1;
	}
	int size = (minode->inode.i_size > 60)? 60 : minode->inode.i_size;
	memcpy(buf, minode->inode.i_block, minode->inode.i_size);
	buf[minode->inode.i_size] = 0;
	return size;
}

#endif