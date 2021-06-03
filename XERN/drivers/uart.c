
#include "uart.h"
#include "../include/ic.h"

struct __UART UART_COM0;
struct __UART *p_com0 = &UART_COM0;

static void uart_set_base(struct __UART *uart, int base)
{ 
    uart->base = (char *)base;            
}
static void uart_set_baudrate(struct __UART *uart, int rate)
{ 
    *(uart->base + BAUD_RATE) = rate;     
}
static void uart_enable_interrupts(struct __UART *uart, int interrupts)
{ 
    *(uart->base + INT_MASK) = interrupts; 
}
static void uart_set_datareg(struct __UART *uart, char c)             
{ 
    *(uart->base + DATA_REG) = c;         
}
static char uart_get_datareg(struct __UART *uart)                     
{  
    return *(uart->base + DATA_REG);     
}
static int uart_get_flag(struct __UART *uart)                         
{  
    return *(uart->base + FLAG_REG);
}

void UART0_init()
{
    PIC_INT_CTRL |= (1<<12); // UART0 at bit 12  

    uart_set_base(p_com0, UART0_ADDR);
    uart_set_baudrate(p_com0, R_9600);
    uart_enable_interrupts(p_com0, RX_INTERRUPT);

    p_com0->ex_get = p_com0->key_avail = 0;
}


volatile int is_special = 0;
char CTRL_C = 0x3;

void UART0_handler()
{
    extern struct proc *curr_proc;
    extern unsigned int slimit;          // Check 'kfuncs.c'
    extern unsigned int user_exit_addr;  // Check 'kill.c'
    
    while((uart_get_flag(p_com0) & RX_FULL) == 0);
    char c = uart_get_datareg(p_com0);
    if(c == CTRL_C && curr_proc->pid > 1)   
    {
        kputs("^C");
        if(curr_proc->pid == 2) // don't add first instance of the sh program
        {
            c = '\r';
            p_com0->ex_get = 1;
            goto do_operation;
        }
        if((get_spsr() & 0x1F) == 0x13)  // if coming from SVC mode
        {
            slimit = 0;  // To stop sleep syscall if currently in use 
            curr_proc->tf->lr = user_exit_addr;
        }

        if((get_spsr() & 0x1F) == 0x10) // if coming from USR mode 
                exit_currproc();
    
        if(p_com0->ex_get)
        {
            c = '\r';
            goto do_operation;
        }
        kputs("\n");
        return;
    }

    /* A simple way to identify special keys like 'delete' */
    if((int)c != 0x5B && (int)c != 0x4F && is_special == 1) // Just the ESCKEY being pressed
    {
        is_special = 0;
        goto do_operation;
    }

    if((int)c == 0x1B)
    {
        is_special = 1;
        return;
    }
    if(is_special == 1)     // 1st character in sequence
    {
        is_special++;
        return;
    }
    
    if(is_special >= 2) // 2nd or 3rd char in sequence
    {
        if(is_special == 2) // 2nd char in sequence
        {
            is_special++;
            return;
        }
        is_special = 0;
    }
    
 
    do_operation:
        if(p_com0->ex_get)
        {
            disable_irq();
            p_com0->ex_get = 0;
            p_com0->input_key = c;
            p_com0->key_avail = 1;
            enable_irq();
        }
}


char uart_getc()
{
    p_com0->ex_get = 1;
    while(p_com0->key_avail == 0);
    disable_irq();
    p_com0->ex_get = 0;
    char c = p_com0->input_key;
    p_com0->key_avail = 0;
    enable_irq();
    return c;
}

void uart_putc(char c)
{
    while(uart_get_flag(p_com0) & TX_FULL);
    uart_set_datareg(p_com0, c);
}


void kputs(char *s)
{
    while(*s)
    {
        uart_putc(*s);
        if(*s == '\n')
            uart_putc('\r');
        s++;
    }

}

static void kprinthex(unsigned int value)
{
    char digits[] = "0123456789ABCDEF";
    char tmp[10], out[10];
    
    if(value == 0)
    {
        kputs("0x0");
        return;
    }
    int pos = 0;
    kputs("0x");

    do
    {
        tmp[pos] = digits[value%16];
        value = value/16;
        pos++;
    
    }while(value > 0);

    tmp[pos] = 0;
    reverse(tmp, out);
    kputs(out);
}

static void kprintuint(unsigned int value)
{
    char digits[] = "0123456789";
    char tmp[12], out[12];
    
    if(value == 0)
    {
        uart_putc('0');
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
    kputs(out);
}

static void kprintint(int value)
{
    char digits[] = "0123456789";
    char tmp[12], out[12];
    
    if(value == 0)
    {
        uart_putc('0');
        return;
    }
  
    if(value < 0)
    {
        uart_putc('-');
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
    kputs(out);
}


void kprintf(char *format, ...)
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
                    uart_putc((char)*param);
                    if((char)*param == '\n')
                        uart_putc('\r');
                    break;
      
                case 's': 
                    kputs((char *)*param);  
                    break;

                case 'x': 
                    kprinthex((unsigned int)*param);          
                    break;

                case 'u': 
                    kprintuint((unsigned int)*param);          
                    break;
      
                case 'd': 
                    kprintint((int)*param);          
                    break;

                default:
                    uart_putc('%');
            }

            f_ptr++; 
            param++;  
        }
        else
        {
            uart_putc(*f_ptr);
            if(*f_ptr == '\n')
                uart_putc('\r');
            f_ptr++; 
        }
  
    }
}
