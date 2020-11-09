/* The 'PRINT' functions in this file doesn't return a value either on success or error. This is for simplicity */

#define STDIN_FILENO   0
#define STDOUT_FILENO  1
#include "../include/string.h"

/*This function null term*/
int gets(char buf[], int maxlength)
{
    return read(STDIN_FILENO, buf, maxlength);
}

void putc(char c)
{
	write(STDOUT_FILENO, &c, 1);
}

void puts(char *s)
{
	while(*s)
	{
		write(STDOUT_FILENO, s, 1);
		s++;
	}
}

static void printhex(unsigned int value)
{
    char digits[] = "0123456789ABCDEF";
    char tmp[10], out[10];
    
    if(value == 0)
    {
        puts("0x0");
        return;
    }
    int pos = 0;
    puts("0x");

    do
    {
        tmp[pos] = digits[value%16];
        value = value/16;
        pos++;
    
    }while(value > 0);

    tmp[pos] = 0;
    reverse(tmp, out);
    puts(out);
}

static void printuint(unsigned int value)
{
    char digits[] = "0123456789";
    char tmp[12], out[12];
    
    if(value == 0)
    {
        putc('0');
        return;
    }
    int pos = 0;

    do
    {
        tmp[pos] = digits[value%10];
        value = value/10;
        pos++;
    
    }while(value > 0);

    tmp[pos] = 0;
    reverse(tmp, out);
    puts(out);
}

static void printint(int value)
{
    char digits[] = "0123456789";
    char tmp[12], out[12];
    
    if(value == 0)
    {
        putc('0');
        return;
    }
  
    if(value < 0)
    {
        putc('-');
        value = -value;
    }

    int pos = 0;

    do
    {
        tmp[pos] = digits[value%10];
        value = value/10;
        pos++;

    }while(value > 0);

    tmp[pos] = 0;
    reverse(tmp, out);
    puts(out);
}


void printf(char *format, ...)
{
    char *f_ptr = format;
    int *param = (int *)&format + 1;

    while(*f_ptr)
    {
        if(*f_ptr == '%')
        {
            f_ptr++;         
            switch(*f_ptr)
            {
                case 'c': 
                    putc((char)*param);
                    break;
      
                case 's': 
                    puts((char *)*param);  
                    break;

                case 'x': 
                    printhex((unsigned int)*param);          
                    break;

                case 'u': 
                    printuint((unsigned int)*param);          
                    break;
      
                case 'd': 
                    printint((int)*param);          
                    break;

                default:
                    putc('%');
            }

            f_ptr++; 
            param++;  
        }
        else
        {
            putc(*f_ptr);
            f_ptr++; 
        }
  
    }
}

