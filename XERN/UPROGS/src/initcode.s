   .global  _entry, syscall
   .equ SYS_exit, 0x9
.text

_entry:	
	bl main
	mov r0, #SYS_exit  
        eor r1, r1 
	swi #0     @ exit(0)
		
syscall:
	swi #0
	mov pc, lr
