#!/bin/sh
echo "----[ This script is used to compile user binary executables ]---"
srcfile=$1
if [ -z "$srcfile" ]; 
then
	echo "usage: $0  <file> -- Note: Don't add the .c"
	exit
fi

LIBGCC="/usr/lib/gcc/arm-none-eabi/8.3.1/libgcc.a"  # USE: eg: To handle division by zero
arm-none-eabi-as -mcpu=arm926ej-s initcode.s -o initcode.o -w
arm-none-eabi-gcc -c -mcpu=arm926ej-s -o lib/string.o lib/string.c  -w
arm-none-eabi-gcc -c -mcpu=arm926ej-s -o lib/io.o lib/io.c -w
arm-none-eabi-gcc -c -mcpu=arm926ej-s -o lib/usyscalls.o lib/usyscalls.c  -w
arm-none-eabi-gcc -c -mcpu=arm926ej-s -o myprog.o $srcfile.c
arm-none-eabi-ld -T user.ld initcode.o lib/string.o lib/usyscalls.o lib/io.o myprog.o $LIBGCC -Ttext=0x80000000   -o ../bin/$srcfile 

echo "[+] CHECK '../bin' TO SEE COMPILED BINARIES"
rm *.o



 




