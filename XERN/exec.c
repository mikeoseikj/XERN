#include "include/elf.h"
#include "include/fcntl.h"
#include "include/globals.h"

#define MAXARGS      16
#define MAX_ARG_LEN 255

#define OWNER  000700
#define GROUP  000070
#define OTHER  000007

int rights_to_execute(struct minode *mip)
{
    int permbits;
    if(curr_proc->uid == mip->inode.i_uid)
        permbits = (mip->inode.i_mode & OWNER) >> 6;
    else if(curr_proc->gid == mip->inode.i_gid)
        permbits = (mip->inode.i_mode & GROUP) >> 3;
    else 
        permbits = mip->inode.i_mode & OTHER; 

    if(permbits & 1)  
        return 1;     //true

    return 0;      //false
}


char progname[256];

int kexec(char *path, char *argv[])
{
    struct elf32_ehdr ehdr;
    struct elf32_phdr phdr;

    int fd = kopen(path, O_RDONLY);
    if(fd < 0)
       return -1;

    if(kread(fd, &ehdr, sizeof(ehdr)) < sizeof(ehdr))
    {
        kclose(fd);
        return -1;
    }

    if((char)ehdr.e_ident[0] != 0x7F || memcmp((char *)&ehdr.e_ident[1], "ELF", 3) != 0)
    {
        kclose(fd);
        kprintf("Error: [%s: Not an ELF executable]\n", path);
        return -1;
    }

    if(! rights_to_execute(curr_proc->fd[fd]->m_ptr))
    {
        kprintf("Error: [%s: You don\'t have execute permission]\n", path);
        return -1;
    }
  

    struct proc *t_p = &procs[0];  // using proc[0] as a tmp storage for page allocation because it is not used by the kernel

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

        if(allocuvm(t_p, phdr.p_memsz) == 0)
   		   goto error;

        if(! loaduvm(t_p, phdr.p_vaddr, fd, phdr.p_offset, phdr.p_filesz))
    	   goto error;

    } 
    kclose(fd);
  
    /* Allocating 20KiB userspace stack memory but we will delete 1 page (20KiB-4KiB = 16KiB)*/
    unsigned int stack = allocuvm(t_p, PAGE_SIZE+USER_STACK_SIZE);
    if(stack == 0)
        goto error;
    
    clrpage(t_p, stack, PAGE_SIZE);
    stack += PAGE_SIZE;
    t_p->size += PAGE_SIZE; 

    /* For dynamic memory allocation */
    int h_pos = t_p->size/0x100000; 
    int h_off = (t_p->size%0x100000) / PAGE_SIZE;
    t_p->brk_start = (0x80000000 + (h_pos * 0x100000)) + (PAGE_SIZE * h_off);  

    
    int params[1+MAXARGS+1];    
    unsigned int *pgtable = (unsigned int *)((unsigned int)t_p->pgdir[stack/0x100000] & 0xFFFFFC00);
    unsigned int ksp = (pgtable[(stack%0x100000) / PAGE_SIZE] & 0xFFFFF000); //ksp => physical memory
    ksp += USER_STACK_SIZE;   // Note: all stacks grow downward

    basename(path, progname);

    int kused = 0, args_count = 1;
    ksp -= (strlen(path) + 1); 
    kused += (strlen(path) + 1);
    strncpy((char *)ksp, progname, (strlen(progname) + 1));

    params[0] = 0xDEADBEEF; // return address for main() (Note: main will never return)
    params[1] = (stack + USER_STACK_SIZE) - kused;  //user - argv[0]

    if(argv == 0)
        goto NO_ARGVS_PROVIDED;

    for(int i = 0; argv[i]; i++)
    {
        if(args_count == MAXARGS)
            break;

        if(strlen(argv[i]) > MAX_ARG_LEN)
            goto error;

        args_count++;
        ksp -= (strlen(argv[i]) + 1);
        kused += (strlen(argv[i]) + 1);
        strncpy((char *)ksp, (char *)argv[i], strlen(argv[i]));

        params[2+i] = (stack + USER_STACK_SIZE) - kused;
    }

NO_ARGVS_PROVIDED:

    params[args_count+1] = 0;    //Null terminate argv[] 
    ksp -= sizeof(params);
    kused += sizeof(params);
    memcpy((char *)ksp, (char *)params, sizeof(params));

    /* Setting kernel register 'stack' to zeros */

    //curr_proc->tf->r0 = args_count;  // trivial -- return will do it            
    curr_proc->tf->r1 = ((stack + USER_STACK_SIZE) - (kused)) + 4; // execlude the return address for the main 
    curr_proc->tf->lr = ehdr.e_entry;
    curr_proc->tf->sp = (stack + USER_STACK_SIZE) - kused;  
    curr_proc->tf->cpsr = 0x10;    // cpsr = usermode

    strncpy(curr_proc->name, path, 255);
    freeuvm(curr_proc);

    unsigned int *old_pgdir = curr_proc->pgdir;
    curr_proc->pgdir = t_p->pgdir;
    t_p->pgdir = old_pgdir;
    curr_proc->size = t_p->size;
    t_p->size = 0;
    curr_proc->brk_start = t_p->brk_start;

    
    for(int i = 3; i < PROC_FD_MAX; i++)    // close all files except stdin, stdout and stderr
    {
        if(curr_proc->fd[i] != 0)
            kclose(i);
    }

    switchuvm((unsigned int *)curr_proc->pgdir);
    return args_count;  // required because return value will overwrite r0

    error:
        freeuvm(t_p);
        kclose(fd);
        return  0;     //FALSE

}


