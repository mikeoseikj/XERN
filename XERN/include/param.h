#ifndef PARAM_H
#define PARAM_H

/* For the memory fucntions (memory.h) */
#define RAMSZ              258	    // Includes 2MiB I/O memory
#define PAGE_SIZE         4096
#define PAGES_PER_BLK      256
#define USER_STACK_SIZE  16384

#define NPROCS              64
#define STACKSIZE         1024
#define PROC_FD_MAX         10

/* For the filesystem */
#define NOFTS              100
#define BLOCKSIZE         2048
#define HALF_BLOCKSIZE    1024
#define INODES_PER_BLOCK    16
#define NMINODES           128
#define SECS_PER_BLK         4	 // For the inode.i_blocks 

/* For the pipe implementation */
#define NPIPES              24
#define PIPESIZE          8192
#define READ_PIPE            4
#define WRITE_PIPE           5

/* For the lseek implementation */
#define SEEK_SET             0
#define SEEK_CUR             1
#define SEEK_END             2

#endif
