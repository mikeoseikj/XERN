.text

.include "include/asm/modes.s"

	.global reset_handler
reset_handler:
	b _start

	.global undef_handler
undef_handler:
	stmfd sp!, {r0-r12, lr}
	bl undef_chandler
	ldmfd sp!, {r0-r12, lr}
	subs pc, lr, #4


	.global prefetch_abort_handler
prefetch_abort_handler:
	stmfd sp!, {r0-r12, lr}
	bl prefetch_abort_chandler
	ldmfd sp!, {r0-r12, lr}
	subs pc, lr, #4


	.global data_abort_handler
data_abort_handler:
	stmfd sp!, {r0-r12, lr}
	bl data_abort_chandler
	ldmfd sp!, {r0-r12, lr}
	subs pc, lr, #4   


	.global irq_handler
irq_handler:
	stmfd sp!, {r0-r12, lr}
	bl irq_chandler
	ldmfd sp!, {r0-r12, lr}
	subs pc, lr, #4


	.global fiq_handler
fiq_handler:
	stmfd sp!, {r0-r12, lr}
	bl fiq_chandler
	ldmfd sp!, {r0-r12, lr}
	subs pc, lr, #4


	.global swi_handler
swi_handler:
	add sp, sp, #8    	@ space for tf->sp, tf->cpsr
	stmfd sp!, {r0-r12, lr}	@ Store usermode registers

	@@ Remember that r0-r3 contain function arguments so we cant use them
	ldr r4, =curr_proc         
    ldr r5, [r4,#0]    @ proc      
    ldr r4, [r5,#8]    @ ->tf 
    mrs r5, spsr
    str r5, [r4, #60]   @ tf->cpsr
   
    msr cpsr, #SYS_MODE
    str lr, [r4, #52]   @ tf->lr
    str sp, [r4, #56]   @ tf->sp

    mov r5, #SVC_MODE   @ enable only irq interrupts
  	orr r5, #F_BIT
  	msr cpsr, r5
  	bl swi_chandler
  	str r0, [sp, #0]  @ replace usermode retval 
  	b return_to_usermode
  	