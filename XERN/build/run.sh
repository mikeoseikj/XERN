#!/bin/sh
qemu-system-arm -M versatilepb -m 256M -kernel xernel.bin -sd SDCARD  -serial mon:stdio
