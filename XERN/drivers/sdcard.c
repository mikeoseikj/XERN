#include "sdcard.h"
#include "../include/ic.h"

#define SDC_SEC_SIZE  512
#define SDC_BLK_SIZE 2048

static void sdc_setreg(int reg, int value)
{
	*(unsigned int *)(SDC_BASE_ADDR + reg) = value;
}
static unsigned int sdc_getreg(int reg)
{
	unsigned int value = *(unsigned int *)(SDC_BASE_ADDR + reg);
	return value;
}

void sdc_exec_command(int cmd, int arg, int resp)
{
	sdc_setreg(MCI_ARGUMENT, arg);
	sdc_setreg(MCI_COMMAND, 0x400 | (resp<<6) | cmd);
	for(int i = 0; i < 1024; i++);  // delay
}
 
void SDC_init()
{
    PIC_INT_CTRL |= (1<<31);   
    SIC_INT_CTRL |= (1<<22);   

	unsigned int RCA = 0x45670000;

	sdc_setreg(MCI_POWER, 0xBF);
    sdc_setreg(MCI_CLOCK, 0xC6);
    sdc_exec_command(CMD_APP_CMD, 0, MCI_RSP_R1); 
    sdc_exec_command(CMD_APP_SEND_OP_COND, 0xFFFF, MCI_RSP_R3);
  	sdc_exec_command(CMD_ALL_SEND_CID, 0, MCI_RSP_R2); 
    sdc_exec_command(CMD_SET_BLOCKLEN, SDC_SEC_SIZE, MCI_RSP_R1);
    sdc_exec_command(CMD_SET_RELATIVE_ADDR, RCA, MCI_RSP_R1); 
    sdc_exec_command(CMD_SELECT_CARD, RCA, MCI_RSP_R1);
}



void sdc_read(int sector, char *buf)
{
	sdc_setreg(MCI_DATATIMER, 0xFFFFFFFF);
	sdc_setreg(MCI_DATALEN, SDC_SEC_SIZE);
	sdc_setreg(MCI_DATACTRL, 0x93);  

	unsigned int arg = sector * SDC_SEC_SIZE;
	sdc_exec_command(CMD_READ_SINGLE_BLOCK, arg, MCI_RSP_R1);

	unsigned int *ptr = (unsigned int *)buf;
    unsigned int status = sdc_getreg(MCI_STATUS);
    unsigned int status_err = status & (SDI_STA_DCRCFAIL|SDI_STA_DTIMEOUT|SDI_STA_RXOVERR);
 
    int count = 512;
    while (!status_err && count) 
    {
    	while(status & RX_FIFO_FULL == 0); 
      
    	for(int i = 0; i < 16; i++)
    		*(ptr + i) = sdc_getreg(MCI_FIFO);

    	ptr += 16;
    	count -= 64;
    	status = sdc_getreg(MCI_STATUS); // read status to clear Rx interrupt
    	status_err = status & (SDI_STA_DCRCFAIL|SDI_STA_DTIMEOUT|SDI_STA_RXOVERR);
   }
   sdc_setreg(MCI_CLEAR, 0xFFFFFFFF);

}

void sdc_write(int sector, char *buf)
{
	sdc_setreg(MCI_DATATIMER, 0xFFFFFFFF);
	sdc_setreg(MCI_DATALEN, SDC_SEC_SIZE);

	unsigned int arg = sector * SDC_SEC_SIZE;
	sdc_exec_command(CMD_WRITE_SINGLE_BLOCK, arg, MCI_RSP_R1);
	sdc_setreg(MCI_DATACTRL, 0x91);

	unsigned int *ptr = (unsigned int *)buf;
    unsigned int status = sdc_getreg(MCI_STATUS); 
    unsigned int status_err = status & (SDI_STA_DCRCFAIL | SDI_STA_DTIMEOUT);
    
    int count = 512;
    while (!status_err && count) 
    {
		while((status & TX_FIFO_EMPTY) == 0); 
     
     	for(int i = 0; i < 16; i++)
        	sdc_setreg(MCI_FIFO, *(ptr + i));
    
    	ptr += 16;
    	count -= 64;
    	status = sdc_getreg(MCI_STATUS); 
    	status_err = status & (SDI_STA_DCRCFAIL | SDI_STA_DTIMEOUT);
    }
    sdc_setreg(MCI_CLEAR, 0xFFFFFFFF);
}



/* Functions for the reading and writing of 2KiB filesystem blocks */
void getblock(int blk, char *buf)
{
    sdc_read(4*blk, buf);
    sdc_read(4*blk + 1, (char *)buf + 512);
    sdc_read(4*blk + 2, (char *)buf + 1024);
    sdc_read(4*blk + 3, (char *)buf + 1536);
}

void putblock(int blk, char *buf)
{
    sdc_write(4*blk, buf);
    sdc_write(4*blk + 1, buf + 512);
    sdc_write(4*blk + 2, buf + 1024);
    sdc_write(4*blk + 3, buf + 1536);
}
