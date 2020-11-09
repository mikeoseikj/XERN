#ifndef FEXIST
#define FEXIST

int kfexist(char *path)
{
	struct minode *mip = search_inode(path);
	if(mip) 
		return 1;  // Exists
	else    
		return 0;
}


#endif
