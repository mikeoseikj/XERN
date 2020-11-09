#ifndef STAT_H
#define STAT_H

#define S_IFMT        0170000 // these bits determine file type

#define S_IFDIR       0040000 // directory
#define S_IFCHR       0020000 // character device
#define S_IFBLK       0060000 // block device
#define S_IFREG       0100000 // regular file
#define S_IFLNK       0120000 // symbolic link


#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#define S_ISCHR(mode) (((mode) & S_IFMT) == S_IFCHR)
#define S_ISBLK(mode) (((mode) & S_IFMT) == S_IFBLK)
#define S_ISREG(mode) (((mode) & S_IFMT) == S_IFREG)
#define S_ISLNK(mode) (((mode) & S_IFMT) == S_IFLNK)


struct stat
{
	unsigned short st_dev;   	// ID of device containing file (Not used)
	unsigned int   st_ino;   	// Inode number
	unsigned short st_mode;  	// File types and mode
	unsigned short st_nlink; 	// Number of hard links
	unsigned short st_uid;		// User ID of owner
	unsigned short st_gid;		// Group ID of owner 
	unsigned short st_rdev;		// Device ID (if special file)
	unsigned int   st_size;		// Total size, in bytes
	unsigned short st_blksize;	// Block size for filesystem I/O
	unsigned int   st_blocks;   // Number of 512B blocks allocated

	unsigned int st_atim;		// Time of last access
	unsigned int st_mtim;		// Time of last modification
	unsigned int st_ctim;       // Time of status change

};

#endif