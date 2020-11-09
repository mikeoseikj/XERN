#ifndef FCNTL_H
#define FCNTL_H

#define O_RDONLY     00000000
#define O_WRONLY     00000001
#define O_RDWR       00000002
#define O_DIRECTORY  00000004 

#define O_CREAT      00000100      // not fcntl
#define O_TRUNC      00001000      // not fcntl
#define O_APPEND     00002000

#endif