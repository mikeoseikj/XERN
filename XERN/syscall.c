
#include "include/syscall.h"
#include "kfuncs.c"


/* File system functions (Note: Filesystem was linked)*/
extern int  kmkdir();
extern int  kcreat();
extern int  krmdir();
extern int  klink();
extern int  kunlink();
extern int  ksymlink();
extern int  kreadlink();
extern int  kopen();
extern int  kread();
extern int  kwrite();
extern int  kclose();
extern int  klseek();
extern int  kdup2();
extern int  kgetdent();
extern int  kstat();
extern int  kchdir();
extern char *kgetcwd();
extern int  kchown();
extern int  kchmod();
extern int  kfexist();

int (*syscalls[])() = 
{
	[SYS_getpid]     kgetpid,
    [SYS_getppid]    kgetppid,
    [SYS_getuid]     kgetuid,
    [SYS_getgid]     kgetgid,
    [SYS_setuid]     ksetuid,
    [SYS_setgid]     ksetgid,
    [SYS_fork]       kfork,
    [SYS_exec]       kexec,
    [SYS_wait]       kwait,
    [SYS_exit]       kexit, 
    [SYS_pipe]       kpipe,
    [SYS_kill]       kkill,
    [SYS_sleep]      ksleep,
    [SYS_sbrk]       ksbrk,
    [SYS_creat]      kcreat,
    [SYS_mkdir]      kmkdir,
    [SYS_rmdir]      krmdir, 
    [SYS_link]       klink,
    [SYS_unlink]     kunlink,
    [SYS_symlink]    ksymlink, 
    [SYS_readlink]   kreadlink,
    [SYS_open]       kopen,
    [SYS_read]       kread,
    [SYS_write]      kwrite,
    [SYS_close]      kclose,
    [SYS_lseek]      klseek,
    [SYS_dup2]       kdup2,
    [SYS_getdent]    kgetdent,
    [SYS_stat]       kstat,
    [SYS_chdir]      kchdir,
    [SYS_getcwd]     kgetcwd,
    [SYS_chown]      kchown,
    [SYS_chmod]      kchmod,
    [SYS_fexist]     kfexist,
    [SYS_readpass]   kreadpass,
    [SYS_plist]      kget_procs,
    [SYS_btime]      kget_boottime,
    [SYS_setlogin]   ksetlogin,
    [SYS_getlogin]   kgetlogin
};

/* Currently, all the functions uses at most 3 args. So, doing it this way is sufficient */
int swi_chandler(unsigned int number, unsigned int arg1, unsigned int arg2, unsigned int arg3)
{
    if(number > 38) // Current we have only 39 syscalls starting from syscall 0 
    {
        kprintf("[*] Unknown syscall number => %d\n", number);
        return -1;
    }
	int retval = (*syscalls[number])(arg1, arg2, arg3);
    return retval;
}
