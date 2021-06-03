#ifndef UNISTD_H
#define UNISTD_H

#include "dirent.h"
#include "process.h"

/* Kernel functions */
int  syscall(int number, ...);
int  getpid();
int  getppid();
int  getuid();
int  getgid();
int  setuid(int uid);
int  setgid(int gid);
int  fork();
int  exec(char *path, char *argv[]);
int  wait(int *status);
void exit(int value);
int  pipe(int pipefd[]);
int  kill(int pid);
void sleep(int seconds);
char *sbrk(int increment);
int  readpass(char *buf, int count);
int  get_procs(struct process *processes[]);
unsigned int get_boottime();
int  setlogin(char *username);
void getlogin(char buf[]);


/* Filesystem functions */
int  creat(char *path);
int  mkdir(char *path);
int  rmdir(char *path);
int  link(char *oldpath, char *newpath);
int  unlink(char *path);
int  symlink(char *target, char *linkpath);
int  readlink(char *path, char buf[]);
int  open(char *file, int flags);
int  read(int fd, char *buf, int count);
int  write(int fd, char *buf, int count);
int  close(int fd);
int  lseek(int fd, unsigned int offset, int whence);
int  dup2(int oldfd, int newfd);
int  getdent(int fd, struct dirent *dir_entry);
int  stat(char *path, struct stat *statbuf);
int  chdir(char *path);
char *getcwd(char *buf, int size);
int  chown(char *path, int uid, int gid);
int  chmod(char *path, unsigned short mode);
int  fexist(char *path);

#endif
