#ifndef HANDLERS_H
#define HANDLERS_H

void irq_chandler()
{
    int status = PIC_STATUS;
    if(status & (1<<4))
        TIMER0_handler();

    else if(status & (1<<12))
        UART0_handler();
} 

void data_abort_chandler()
{
    int spsr = get_spsr();
    kputs("\x1B[31mDATA ABORT exception:");
   
    if((spsr & 0x1F) == 0x10)
        kputs(" [USR mode]: ");
    else if((spsr & 0x1F) == 0x12)
        kputs(" [IRQ mode]: ");
    else if((spsr & 0x1F) == 0x13)
        kputs(" [SVC mode]: ");

    kprintf("virtual address =>  %x\x1B[0m\n", get_fault_addr());
}


void prefetch_abort_chandler()
{
    kprintf("\x1B[31mPREFETCH ABORT exception: fault addr => %x\x1B[0m\n", get_fault_addr());
}

void undef_chandler()
{
    kprintf("\x1B[31mUNDEFINED exception\x1B[0m\n");
}

void fiq_chandler()
{
    kprintf("FIQ interrupt\n");
}


#endif
