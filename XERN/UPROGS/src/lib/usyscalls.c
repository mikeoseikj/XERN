#include "../include/syscall.h"
#include "../include/dirent.h"

int getpid()
{
    return syscall(SYS_getpid);
}

int getppid()
{
    return syscall(SYS_getppid);
}

int getuid()
{
    return syscall(SYS_getuid);
}

int getgid()
{
    return syscall(SYS_getgid);
}

int setuid(int uid)
{
    return syscall(SYS_setuid, uid);
}

int setgid(int gid)
{
    return syscall(SYS_setgid, gid);
}

int fork()
{
    return syscall(SYS_fork);
}

int exec(char *path, char *argv[])
{
    return syscall(SYS_exec, path, argv);
}

int wait(int *status)
{
    return syscall(SYS_wait, status);
}

void exit(int value)
{
    syscall(SYS_exit, value);
}

int pipe(int pipefd[])
{
    return syscall(SYS_pipe, pipefd);
}

int kill(int pid)
{
    return syscall(SYS_kill, pid);
}

void sleep(int seconds)
{
    syscall(SYS_sleep, seconds);
}

char *sbrk(int increment)
{
    return syscall(SYS_sbrk, increment);
}

int creat(char *path)
{
    return syscall(SYS_creat, path);
}

int mkdir(char *path)
{
    return syscall(SYS_mkdir, path);
}

int rmdir(char *path)
{
    return syscall(SYS_rmdir, path);
}

int link(char *oldpath, char *newpath)
{
    return syscall(SYS_link, oldpath, newpath);
}

int unlink(char *path)
{
    return syscall(SYS_unlink, path);
}

int symlink(char *target, char *linkpath)
{
    return syscall(SYS_symlink, target, linkpath);
}

int readlink(char *path, char buf[])
{
    return syscall(SYS_readlink, path, buf);
}

int open(char *file, int flags)
{
    return syscall(SYS_open, file, flags);
}

int read(int fd, char *buf, int count)
{
    return syscall(SYS_read, fd, buf, count);
}

int write(int fd, char *buf, int count)
{
    return syscall(SYS_write, fd, buf, count);
}

int close(int fd)
{
    return syscall(SYS_close, fd);
}

int lseek(int fd, unsigned int offset, int whence)
{
    return syscall(SYS_lseek, fd, offset, whence);
}

int dup2(int oldfd, int newfd)
{
    return syscall(SYS_dup2, oldfd, newfd);
}

int getdent(int fd, struct dirent *dir_entry)
{
    return syscall(SYS_getdent, fd, dir_entry);
}

int stat(char *path, struct stat *statbuf)
{
    return syscall(SYS_stat, path, statbuf);
}

int chdir(char *path)
{
    return syscall(SYS_chdir, path);
}

char *getcwd(char *buf, int size)
{
    return (char *)syscall(SYS_getcwd, buf, size);
}

int chown(char *path, int uid, int gid)
{
    return syscall(SYS_chown, path, uid, gid);
}

int chmod(char *path, unsigned short mode)
{
    return syscall(SYS_chmod, path, mode);
}

int fexist(char *path)
{
    return syscall(SYS_fexist, path);
}

int readpass(char *buf, int count)
{
    return syscall(SYS_readpass, buf, count);
}

int get_procs(struct process *processes[])
{
    return syscall(SYS_plist, processes);
}

unsigned int get_boottime()
{
    return syscall(SYS_btime);
}

int setlogin(char *username)
{
    return syscall(SYS_setlogin, username);
}

void getlogin(char buf[])
{
    syscall(SYS_getlogin, buf);
}