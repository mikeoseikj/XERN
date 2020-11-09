.text
.include "include/asm/modes.s"

  .global _start   

_start:
    ldr sp, =svc_stack_top   @ Initial kernel stack during boot. Each process has its own kernel stack memory

@@ Go to various modes to set stack pointer with IRQ and FIQ disabled
    mov r0, #FIQ_MODE
    orr r0, #I_BIT
    orr r0, #F_BIT 
    msr cpsr, r0
    ldr sp, =fiq_stack_top

    mov r0, #IRQ_MODE
    orr r0, #I_BIT
    orr r0, #F_BIT 
    msr cpsr, r0
    ldr sp, =irq_stack_top

    mov r0, #ABT_MODE
    orr r0, #I_BIT
    orr r0, #F_BIT 
    msr cpsr, r0
    ldr sp, =abt_stack_top

    mov r0, #UND_MODE
    orr r0, #I_BIT
    orr r0, #F_BIT 
    msr cpsr, r0
    ldr sp, =und_stack_top

@@ Switch back to supervisor mode with IRQ enabled and FIQ disabled
    mov r0, #SVC_MODE
    orr r0, #F_BIT
    bic r0, #I_BIT
    msr cpsr, r0


    bl setup_vector_table
    bl initialize_mmu
    bl kmain
    b .     @ System hangs


@@@---------------------------------------------------------------------------

vector_table:
    ldr pc, rh
    ldr pc, uh
    ldr pc, sh
    ldr pc, ph
    ldr pc, dh
    b .
    ldr pc, ih
    ldr pc, fh

rh: .word reset_handler
uh: .word undef_handler
sh: .word swi_handler
ph: .word prefetch_abort_handler
dh: .word data_abort_handler
ih: .word irq_handler
fh: .word fiq_handler
    .equ VT_SIZE, (. - vector_table)


setup_vector_table:
    mov r0, #0
    ldr r1, =vector_table
    mov r2, #VT_SIZE
 vloop:
    ldr r3, [r1], #4
    str r3, [r0], #4
    cmp r0, r2
    bne vloop
    mov pc, lr


@@@---------------------------------------------------------------------------  


  .global switch_context
switch_context:
	stmfd	sp!, {r0-r12, lr}
    ldr r0, =curr_proc         
	ldr r1, [r0,#0]    @ proc      
    ldr r0, [r1,#4]    @ ->context  
    str sp, [r0,#0]   @ ->sp
	bl	scheduler
    ldr r0, =curr_proc
    ldr r1, [r0,#0]    @ proc
    ldr r0, [r1,#4]    @ ->context    
    ldr sp, [r0,#0]   @ ->sp 
    ldmfd	sp!, {r0-r12, pc}


  .global return_to_usermode
return_to_usermode:
    mrs r0, cpsr     @ disable interrupts
    orr r0, #I_BIT
    orr r0, #F_BIT
    msr cpsr, r0

    ldr r0, =curr_proc         
    ldr r1, [r0,#0]    @ proc      
    ldr r0, [r1,#8]    @ ->tf 
    ldr r2, [r0, #60]  @ tf->cpsr
    msr spsr, r2

    msr cpsr, #SYS_MODE
    ldr sp, [r0, #56]   @ tf->sp
    msr cpsr, #SVC_MODE
    ldmfd sp!, {r0-r12, lr}
    subs sp, sp, #8    
    movs pc, lr


  .global switchuvm
switchuvm:
	mcr p15, 0, r0, c2, c0, 0
	mov r1, #0
	mcr p15, 0, r1, c8, c7, 0
	mcr p15, 0, r1, c7, c10, 0
	mrc p15, 0, r2, c2, c0, 0
	mov r0, #0x5
	mcr p15, 0, r0, c3, c0, 0
	mov pc, lr


@@@------------------------------------------------------------------------------------

  .global enable_irq  
enable_irq:	
    mrs r0, cpsr
    bic r0, #I_BIT   
    msr cpsr, r0
    mov pc, lr	

  .global disable_irq  
disable_irq:	
    mrs r0, cpsr
    orr r0, #I_BIT     
    msr cpsr, r0
    mov pc, lr	


  .equ user_exit_addr, 0x80000004   @ disassemble user programs to check it
  .global exit_currproc             @ for CTRL+C
exit_currproc:
    mrs r1, cpsr
    msr cpsr, #SYS_MODE
    mov pc, #user_exit_addr
    msr cpsr, r1
    mov pc, lr


@@@---------------------------------------------------------------------------------------

  .global get_spsr   @ used to check the previous CPU mode
get_spsr:
    mrs r0, spsr
    mov pc, lr

  .global get_fault_addr
get_fault_addr:
    mrc p15, 0, r0, c6, c0, 0
	mov pc, lr

@@@----------------------------------------------------------------------------------------

