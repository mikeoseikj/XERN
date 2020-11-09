#ifndef TIMER_H
#define TIMER_H

#define TIMER0_ADDR  0x101E2000
#define TIMER1_ADDR  0x101E2020
#define TIMER2_ADDR  0x101E3000
#define TIMER3_ADDR  0x101E3020

/* Registers */
#define LOAD_REG   0x00
#define CVAL_REG   0x04
#define CTRL_REG   0x08
#define INT_CLR    0x0C
#define INT_STAT   0x10
#define INT_MASK   0x14
#define BLOAD_REG  0x18


/* Control register bit assignments */
#define CTRL_ONESHOT     0x01
#define CTRL_SIZE        0x02
#define CTRL_INT_ENABLE  0x20
#define CTRL_MODE        0x40
#define CTRL_ENABLE      0x80

volatile struct __TIMER
{
	char *base;
	unsigned int seconds;
};

void TIMER0_init();
void TIMER0_handler();

#endif