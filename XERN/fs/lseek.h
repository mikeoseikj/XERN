#ifndef LSEEK_H
#define LSEEK_H


int klseek(int fd, unsigned int offset, int whence)
{
	if(fd < 0 || fd >= PROC_FD_MAX)
		return -1;

	struct oft *ofptr = curr_proc->fd[fd];

	if(! ofptr)
		return -1;

	if(! S_ISREG(ofptr->m_ptr->inode.i_mode)) // lseek on only regular files
		return -1;

	if(offset > ofptr->m_ptr->inode.i_size)
		offset = ofptr->m_ptr->inode.i_size;

	if(whence == SEEK_SET)
		ofptr->offset = offset;
	else if(whence == SEEK_CUR)
		ofptr->offset += offset;
	else if(whence == SEEK_END)
		ofptr->offset = ofptr->m_ptr->inode.i_size + offset;
	else 
		return -1;

	return ofptr->offset;
}

#endif
