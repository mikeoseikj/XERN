#ifndef DEVICE_H
#define DEVICE_H

#define NDEV 3

struct devsw
{
	int (*read)();
	int (*write)();
};


void do_nothing(){}

struct devsw devsw[NDEV] =
{
	do_nothing,   do_nothing,	  /* /dev/null */
	console_read, console_write,  /* /dev/tty0 */
	serial_read,  serial_write    /* /dev/ttyS0 */
};




#endif