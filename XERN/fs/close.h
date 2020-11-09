#ifndef CLOSE_H
#define CLOSE_H


int kclose(int fd)
{
	if(fd < 0 || fd >= PROC_FD_MAX)  
		return -1;

	struct oft *oft_ptr = curr_proc->fd[fd];

	if(! oft_ptr)
		return -1;

	if((oft_ptr->mode == READ_PIPE) || (oft_ptr->mode == WRITE_PIPE))
		return close_pipe(fd);

    /*We cant just deallocate OFT becuase of increase of refcount in fork()*/
	curr_proc->fd[fd] = 0;
	oft_ptr->refcount--;

	if(oft_ptr->refcount > 0)
		return 0;

	oft_dalloc(oft_ptr);
	return 0;
}
#endif