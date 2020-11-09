#include "include/globals.h"

void pipe_init()
{
	for(int i = 0; i < NPIPES; i++)
		pipes[i].used = 0;
}

int kpipe(int pipefd[])
{
	struct pipe *pipe_ptr = 0;
	struct oft *poft[2];		// for our two pipes 

	for (int i = 0; i < NPIPES; ++i)
	{
		if(pipes[i].used == 0)
		{
			pipe_ptr = &pipes[i];
			pipe_ptr->used = 1;
			break;
		}
	}
	if(! pipe_ptr)
	{
		kprintf("Error: [System out of pipes]\n");
		return -1;
	}

	poft[0] = oft_alloc();
	poft[1] = oft_alloc();

	if(!poft[0] || !poft[1])
	{
		if(poft[0]) oft_dalloc(poft[0]);
		if(poft[1]) oft_dalloc(poft[1]);

		kprintf("Error: [System out of OFTs]\n");
		pipe_ptr->used = 0;
		return -1;
	}

    int pos = 0;
    int fds[2];
	for(int file_desc = 0; file_desc < PROC_FD_MAX; file_desc++)
	{
		if(curr_proc->fd[file_desc] == 0)
		{
			fds[pos] = file_desc;
			pos++;
		}
		if(pos == 2)
			break;
	}
	if(pos < 2)
	{
		oft_dalloc(poft[0]);
		oft_dalloc(poft[1]);

		pipe_ptr->used = 0;
		kprintf("Error: [Process MAX FD reached]\n");
		return -1;
	}

	pipe_ptr->r_off = pipe_ptr->w_off = pipe_ptr->data = 0;

	poft[0]->mode = READ_PIPE;
	poft[1]->mode = WRITE_PIPE;

	poft[0]->p_ptr = poft[1]->p_ptr = pipe_ptr;

	poft[0]->m_ptr = poft[1]->m_ptr = 0;
	pipe_ptr->nread = pipe_ptr->nwrite = 1;

	curr_proc->fd[fds[0]] = poft[0];
	curr_proc->fd[fds[1]] = poft[1];


	pipefd[0] = fds[0];
	pipefd[1] = fds[1];

	return 0;	// SUCCESS
}


int read_pipe(int fd, char *buf, int count)
{
	struct oft *oft_ptr = curr_proc->fd[fd];
	struct pipe *pipe_ptr = oft_ptr->p_ptr;
  
	if(count < 0)
		return -1; 
   
	count = (pipe_ptr->data > count)? count : pipe_ptr->data;
	try_again:
	for(int i = 0; i < count; i++)
	{  
	   buf[i] = pipe_ptr->buf[pipe_ptr->r_off++ % PIPESIZE];
	   pipe_ptr->data--;
	}
    
    if(count)
    {
    	buf[count] = 0;
    	return count;
    }

    if(pipe_ptr->nwrite > 0)
    {
		sleep(&pipe_ptr->data);
		goto try_again;
    }
    
    return -1;
}

int write_pipe(int fd, char *buf, int count)
{
	struct oft *oft_ptr = curr_proc->fd[fd];
	struct pipe *pipe_ptr = oft_ptr->p_ptr;

	if(pipe_ptr->nread == 0)
	{
		kprintf("Error: [FD[%d] : Broken pipe]\n", curr_proc->pid);
		return -1;
	}
    count = (count > strlen(buf))? strlen(buf) : count;
	for(int i = 0; i < count; i++)
	{
		pipe_ptr->buf[pipe_ptr->w_off++ % PIPESIZE] = buf[i];
        pipe_ptr->data++;
	}
	wakeup(&pipe_ptr->data);
	return count;	
}

int close_pipe(int fd)
{
	struct oft *oft_ptr = curr_proc->fd[fd];
	struct pipe *pipe_ptr = oft_ptr->p_ptr;

	curr_proc->fd[fd] = 0;

	if(oft_ptr->mode == READ_PIPE)
	{
		pipe_ptr->nread--;
		oft_ptr->refcount--;

		if(oft_ptr->refcount == 0)
		{
			if(pipe_ptr->nwrite <= 0)
				pipe_ptr->used = 0;
		}
		return 0;
	}
	else if(oft_ptr->mode == WRITE_PIPE)
	{
		pipe_ptr->nwrite--;
		oft_ptr->refcount--;

		if(oft_ptr->refcount == 0)
		{
			if(pipe_ptr->nread <= 0)
			{
				pipe_ptr->used = 0;
				return 0;
			}
		}
		wakeup(&pipe_ptr->data);
		return 0;
	}
	else
	{
		return -1;
	}

}