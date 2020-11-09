ENTRY(reset_handler)
SECTIONS
{
    . = 0x10000;
    .text : {
      start.o
      mmu.o
      handlers.o
      timer.o
      uart.o
      sdcard.o
      filesystem.o
      *(.text)
}
.data : { 
      start.o
      mmu.o
      handlers.o
      timer.o
      uart.o
      sdcard.o
      filesystem.o
      *(.data)
      
  }
 .bss : { 
      start.o
      mmu.o
      handlers.o
      timer.o
      uart.o
      sdcard.o
      filesystem.o
      *(.bss)
  }
  
  . = ALIGN(8);
  . = . + 0x1000;       /* 4kB of svc stack memory */
  svc_stack_top = .;
  . = . + 0x1000;       
  fiq_stack_top = .;
    . = . + 0x1000;   
  irq_stack_top = .;
  . = . + 0x1000;      
  abt_stack_top = .;
  . = . + 0x1000;      
  und_stack_top = .;
 
}

