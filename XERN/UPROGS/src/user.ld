ENTRY(_entry)
SECTIONS
{
	. = 0x80000000;
	.text : { 
	      initcode.o
	      *(.text) 
	 }
	.data : { 
	myprog.o
	*(.data) 
	}
	.bss  : { 
	myprog.o
	*(.bss)  
	}
}



