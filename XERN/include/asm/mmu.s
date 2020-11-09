@@@ We will set 1-level paging and later switch to 2-level paging.
@@@ The switch will be done for the sake of simplicity
.text

.global initialize_mmu

.align 4
addr:  .word 0x4000
val:   .word 0x412
ramsz: .word 258


initialize_mmu:
    ldr r0, val
    eor r1, r1, r1
    ldr r2, addr
    ldr r3, ramsz

@@ Prepare page table (the RAM is (256MiB + 2MiB I/O) in QEMU)
  setmem:
    str r0, [r2], #4
    add r0, r0, #0x100000
    add r1, r1, #1
    cmp r1, r3
    bne setmem
    eor r0, r0, r0
   zloop:             @ Set the rest of the entries in the table to zeros
    str r0, [r2], #4
    add r1, r1, #1
    cmp r1, #4096
    bne zloop


	ldr r0, addr
	mcr p15, 0, r0, c2, c0, 0   @ Set translation table base
	mcr p15, 0, r0, c8, c7, 0	@ flush TTB

@@ Set domain -- Client=> (Check access based on permission bits)
	mov r0, #0x05
	mcr p15, 0, r0, c3, c0, 0


@@ Enabling the MMU
	mrc p15, 0, r0, c1, c0, 0
	orr r0, r0, #0x00000001
	mcr p15, 0, r0, c1, c0, 0
	nop
	nop
	nop
    mrc p15, 0, r2, c2, c0
    mov r2, r2
	mov pc, lr

