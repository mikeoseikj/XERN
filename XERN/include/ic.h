#ifndef IC_H
#define IC_H

#define PIC_BASE_ADDR 0x10140000
#define PIC_STATUS     (*((volatile unsigned int *)(PIC_BASE_ADDR + 0x000)))
#define PIC_INT_CTRL   (*((volatile unsigned int *)(PIC_BASE_ADDR + 0x010)))

#define SIC_BASE_ADDR 0x10003000
#define SIC_STATUS     (*((volatile unsigned int *)(SIC_BASE_ADDR + 0x000)))
#define SIC_INT_CTRL   (*((volatile unsigned int *)(SIC_BASE_ADDR + 0x008)))

#endif
