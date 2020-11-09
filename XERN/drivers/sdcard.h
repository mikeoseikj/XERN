#ifndef SDCARD_H
#define SDCARD_H

#define SDC_BASE_ADDR  0x10005000 

#define RX_FIFO_FULL   (1<<17)
#define TX_FIFO_EMPTY  (1<<18)

/* Used MCI registers */
#define MCI_POWER       0x00
#define	MCI_CLOCK       0x04
#define	MCI_ARGUMENT    0x08
#define	MCI_COMMAND     0x0C
#define	MCI_RESPCMD     0x10
#define	MCI_RESP0       0x14
#define	MCI_RESP1       0x18
#define	MCI_RESP2       0x1C
#define	MCI_RESP3       0x20
#define	MCI_DATATIMER   0x24
#define	MCI_DATALEN     0x28
#define	MCI_DATACTRL    0x2C
#define	MCI_DATACNT     0x30
#define	MCI_STATUS      0x34
#define	MCI_CLEAR       0x38
#define	MCI_MASK0       0x3C
#define	MCI_MASK1       0x40
#define	MCI_SELECT      0x44
#define	MCI_FIFOCNT     0x48
#define	MCI_FIFO        0x80


/* Needed sdc commands */
#define CMD_IDLE_STATE		      0
#define CMD_SEND_OP_COND		  1
#define CMD_ALL_SEND_CID		  2
#define CMD_SET_RELATIVE_ADDR	  3
#define CMD_SELECT_CARD		      7
#define CMD_SEND_EXT_CSD		  8
#define CMD_SET_BLOCKLEN	     16
#define CMD_READ_SINGLE_BLOCK    17
#define CMD_WRITE_SINGLE_BLOCK	 24
#define CMD_APP_SEND_OP_COND     41
#define CMD_APP_CMD			     55

#define MCI_RSP_PRESENT     0x1
#define MCI_RSP_136	        0x2	
#define MCI_RSP_CRC	        0x4		
#define MCI_RSP_BUSY	    0x8		
#define MCI_RSP_OPCODE	   0x10		

/* Response registers */
#define MCI_RSP_NONE	0
#define MCI_RSP_R1	    (MCI_RSP_PRESENT|MCI_RSP_CRC|MCI_RSP_OPCODE)
#define MCI_RSP_R1B	    (MCI_RSP_PRESENT|MCI_RSP_CRC|MCI_RSP_OPCODE| MCI_RSP_BUSY)
#define MCI_RSP_R2	    (MCI_RSP_PRESENT|MCI_RSP_136|MCI_RSP_CRC)
#define MCI_RSP_R3	    (MCI_RSP_PRESENT)
#define MCI_RSP_R4	    (MCI_RSP_PRESENT)
#define MCI_RSP_R5	    (MCI_RSP_PRESENT|MCI_RSP_CRC|MCI_RSP_OPCODE)
#define MCI_RSP_R6	    (MCI_RSP_PRESENT|MCI_RSP_CRC|MCI_RSP_OPCODE)
#define MCI_RSP_R7	    (MCI_RSP_PRESENT|MCI_RSP_CRC|MCI_RSP_OPCODE)


/* SCI status register bits */
#define SDI_STA_DCRCFAIL	0x00000002
#define SDI_STA_DTIMEOUT	0x00000008
#define SDI_STA_RXOVERR		0x00000020

void SDC_init();

#endif
