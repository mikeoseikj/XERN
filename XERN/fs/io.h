/*
	No driver is written for a specific keyboard and LCD. 
  @--------------------------------------------------------------@
	uART is also used for the console read and write. 
*/


#ifndef IO_H
#define IO_H

int serial_write(char *buf, int count)
{
	char *bptr = buf;
	for(int i = 0; i < count; i++)
	{
		uart_putc(*bptr);
		if(*bptr == '\n')
			uart_putc('\r');
		bptr++; 
	}

	return count;
}



int serial_read(char *buf, int count)
{
	char c, *bptr = buf;
	int nread = 0;  // bytes read
	while(1)
	{
		c = uart_getc();
		
		if(c == 0x7F)  // backspace
		{
			if(nread > 0)
			{
				kputs("\b \b");  
				bptr--, nread--;
			}
			continue;
		}
		if(c == '\r')
		{
			*bptr = 0;
			uart_putc('\n');
			uart_putc('\r');
			return nread;
		}
		uart_putc(c);
		*bptr++ = c, nread++;	
		if(nread == count)
		{
			*bptr = 0;
			uart_putc('\n');
			uart_putc('\r');
			return count;
		}
	}
}


int console_write(char *buf, int count)
{
	return serial_write(buf, count);
}


int console_read(char *buf, int count)
{
	return serial_read(buf, count);
}


#endif
