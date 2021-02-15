CC = arm-none-eabi-gcc
CFLAGS = -c -mcpu=arm926ej-s
LIBGCC = /usr/lib/gcc/arm-none-eabi/9.2.1/libgcc.a
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

OBJS = start.o mmu.o handlers.o timer.o uart.o sdcard.o filesystem.o main.o

all: $(OBJS) build_kernel clean

start.o: asm/start.s
	$(CC) $(CFLAGS) asm/start.s

mmu.o: include/asm/mmu.s
	$(CC) $(CFLAGS) include/asm/mmu.s

handlers.o: include/asm/handlers.s
	$(CC) $(CFLAGS) include/asm/handlers.s

timer.o: drivers/timer.c
	$(CC) $(CFLAGS) drivers/timer.c

uart.o: drivers/uart.c
	$(CC) $(CFLAGS) drivers/uart.c

sdcard.o: drivers/sdcard.c
	$(CC) $(CFLAGS) drivers/sdcard.c

filesystem.o: fs/filesystem.c
	$(CC) $(CFLAGS) fs/filesystem.c

main.o: main.c
	$(CC) $(CFLAGS) main.c

build_kernel:
	$(LD) -T kernel.ld $(OBJS) $(LIBGCC) -Ttext=0x10000 -o kernel.elf
	$(OBJCOPY) -O binary kernel.elf build/xernel.bin

clean:
	rm -f $(OBJS) kernel.elf

