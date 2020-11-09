#ifndef TYPES_H
#define TYPES_H

#include "ext2.h"
#include "proc.h"
#include "param.h"


struct minode
{
	int ino;
	int  refcount;
	struct ext2_inode inode;
};

struct oft     // file table
{
	int mode;
	int refcount;
	struct minode *m_ptr;
	struct pipe *p_ptr;
	int offset;
};


struct pipe
{
	char  buf[PIPESIZE];
	int   r_off, w_off;   // read and write buffer offsets
  	int   nwrite, nread;  // number of writers and readers
  	int   data;
  	int   used;
};

#endif