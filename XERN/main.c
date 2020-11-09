#include "include/memory.h"         /* allouvm(), freeuvm() etc */
#include "include/ic.h"
#include "include/globals.h" 
#include "include/string.h"
#include "include/sync.h"           /* ksleep() and kwakeup() */
#include "include/handlers.h"  /* C functions used directly by CPU modes handlers*/

#include "drivers/sdcard.h"
#include "drivers/uart.h"
#include "drivers/timer.h"

/* Including the '.c' files directly makes compilation and linker script simple */
#include "sysinit.c"
#include "initproc.c"
#include "exit.c"
#include "wait.c"
#include "exec.c"
#include "fork.c"
#include "pipe.c"
#include "kill.c"
#include "sbrk.c"
#include "syscall.c"

void kmain()
{ 
    TIMER0_init();
    UART0_init();
	SDC_init(); 
    FS_init();
    OS_init();
    
    if(! start_init_process("/bin/init"))
    {
        kprintf("\x1B[1;31mSystem failure: Unable to start init process\n");
        kprintf("[*************SYSTEM HANGED****************]\x1B[0m\n");
        return; 
    }

    switch_context(); // To the init process
}
