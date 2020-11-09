#ifndef DIRENT_H
#define DIRENT_H

struct dirent
{
	int d_namelen;    /* Length of entry name */
	char d_name[256]; /* Null-terminated filename */

};

#endif