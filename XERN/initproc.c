#include "include/elf.h"
#include "include/fcntl.h"
#include "include/globals.h"

extern unsigned int return_to_usermode();

int start_init_process(char *path)
{
    struct elf32_ehdr ehdr;
    struct elf32_phdr phdr;

    int fd = kopen(path, O_RDONLY);
    if(fd < 0)
       return 0;  // FALSE

    if(kread(fd, &ehdr, sizeof(ehdr)) < sizeof(ehdr))
        return 0;


    if((char)ehdr.e_ident[0] != 0x7F || memcmp((char *)&ehdr.e_ident[1], "ELF", 3) != 0)
        return 0; // FALSE

   struct proc *np = allocproc();
    if(! np)
      return 0; // FALSE

    strcpy(curr_proc->owner_name, "no_name");
    np->state = RUNNABLE;
    np->priority = 1;
    np->parent = curr_proc;
    np->ppid = curr_proc->pid;
    np->size = 0;

    strncpy(np->name, path, 255);

    int off;
    for(int i = 0, off = ehdr.e_phoff; i < ehdr.e_phnum; i++, off += sizeof(phdr))
    {
   	    klseek(fd, off, SEEK_SET);
        if(kread(fd, (char *)&phdr, sizeof(phdr)) < sizeof(phdr))
  		    goto error;

        if(phdr.p_type != ELF_PROG_LOAD)
    	   continue;

        if(phdr.p_memsz < phdr.p_filesz)
    	   goto error;

        if(allocuvm(np, phdr.p_memsz) == 0)
   		   goto error;

        if(! loaduvm(np, phdr.p_vaddr, fd, phdr.p_offset, phdr.p_filesz))
    	   goto error;

    } 

    /* Allocating 16KiB userspace stack memory */
    unsigned int stack = allocuvm(np, PAGE_SIZE+USER_STACK_SIZE);
    if(stack == 0)
        goto error;

    clrpage(np, stack, PAGE_SIZE);
    stack += PAGE_SIZE;
    np->size += PAGE_SIZE;

     /* For dynamic memory allocation */
    int h_pos = np->size/0x100000; 
    int h_off = (np->size%0x100000) / PAGE_SIZE;
    np->brk_start = (0x80000000 + (h_pos * 0x100000)) + (PAGE_SIZE * h_off);  

    np->context->sp = &(np->kstack[STACKSIZE - 30]);  // exclude context->sp

    /* Setting kernel register 'stack' to zeros */
    np->context->r0 = np->context->r1 = np->context->r2 = 0; 
    np->context->r3 = np->context->r4 = np->context->r5 = 0;
    np->context->r6 = np->context->r7 = np->context->r8 = 0;
    np->context->r9 = np->context->r10 = np->context->r11 = 0;
    np->context->r12 = 0;

    np->context->lr = (unsigned int)return_to_usermode;

    /* Setting userspace registers 'stack' to zeros */
    np->tf->r0 = np->tf->r1 = np->tf->r2 =  np->tf->r3 = 0; 
    np->tf->r4 = np->tf->r5 = np->tf->r6 = np->tf->r7 = 0;
    np->tf->r8 = np->tf->r9 = np->tf->r10 = np->tf->r11 = 0;
    np->tf->r12 = 0;

    np->tf->lr = ehdr.e_entry;
    np->tf->sp =  stack + USER_STACK_SIZE;  // Note: stack grows downwards
    np->tf->cpsr = 0x10;    // cpsr = usermode

    for(int i = 0; i < PROC_FD_MAX; i++)
        np->fd[i] = 0;

    enqueue(&runnablequeue, np);
    kclose(fd);
    return 1;  // TRUE 

    error:
        kclose(fd);
        freeuvm(np);
        dallocproc(np);
        return  0;     //FALSE

}
