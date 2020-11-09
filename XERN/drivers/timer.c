#include "timer.h"
#include "../include/ic.h"

struct __TIMER TIMER0;
struct __TIMER *p_time0 = &TIMER0;

static void timer_set_base(struct __TIMER *timer, int base)
{
	timer->base = (char *)base;
}

static void timer_setreg(struct __TIMER *timer, int reg, int value)
{
	*(int *)(timer->base + reg) = value;
}


void TIMER0_init()
{
    PIC_INT_CTRL |= (1<<4);  // TIMER0 at bit 4
	p_time0->seconds = 0;

	timer_set_base(p_time0, TIMER0_ADDR);
	timer_setreg(p_time0, LOAD_REG, 0x400);
	timer_setreg(p_time0, CVAL_REG, 0xFFFFFFFF); 
	timer_setreg(p_time0, INT_STAT, 0);
	timer_setreg(p_time0, INT_MASK, 0);    
	timer_setreg(p_time0, BLOAD_REG, 0xF0000);  // Trial and error value since we are emulating(QEMU)
	timer_setreg(p_time0, CTRL_REG, CTRL_MODE|CTRL_SIZE|CTRL_INT_ENABLE|CTRL_ENABLE);	// In periodic mode & 32 bit counter
}

void TIMER0_handler()
{
	p_time0->seconds++;
	timer_setreg(p_time0, INT_CLR, 0);	// clear interrupt
}