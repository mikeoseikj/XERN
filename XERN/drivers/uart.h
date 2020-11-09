#ifndef  UART_H
#define  UART_H

#include "../include/globals.h"

/* Base address for the 4 devices for serial IO */
#define UART0_ADDR  0x101F1000
#define UART1_ADDR  0x101F2000
#define UART2_ADDR  0x101F3000
#define UART3_ADDR  0x10009000

#define TX_FULL 0x20
#define RX_FULL 0x40

#define UART_BUFLEN  256

/* Registers */
#define DATA_REG   0x00
#define STAT_REG   0x04
#define FLAG_REG   0x18
#define BAUD_RATE  0x24
#define CTRL_REG   0x2C
#define INT_MASK   0x38
#define INT_STAT   0x40

/* Baud rates */
#define R_1152000  0x4
#define R_38400    0xC
#define R_192000  0x18 
#define R_14400   0x20 
#define R_9600    0x30

/* For enabling interrupts */
#define RX_INTERRUPT  (1<<4)
#define TX_INTERRUPT  (1<<5)

volatile struct __UART
{
    char *base;
    char input_key;
    int ex_get, key_avail;  // for ex_get => we're explicitly getting a key
};


void UART0_init();
void UART0_handler();

char kgetc();
void kputc(char c);
void kputs(char *s);
void kprintf(char *format, ...);

#endif