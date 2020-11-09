@@@ Definitions of used CPU modes and the interrupts bits

.equ  I_BIT, 0x80      @ If I=1 IRQ is disabled
.equ  F_BIT, 0x40
.equ  USR_MODE, 0x10 
.equ  FIQ_MODE, 0x11 
.equ  IRQ_MODE, 0x12
.equ  SVC_MODE, 0x13
.equ  ABT_MODE, 0x17
.equ  UND_MODE, 0x1B
.equ  SYS_MODE, 0x1F
