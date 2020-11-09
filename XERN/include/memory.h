#ifndef MEMORY_H
#define MEMORY_H

#include "string.h"

/* Note that i used a RAM size if 256MiB in QEMU */

/* 

@@@-----------------------------------------------------------------------@@@
    About 4MiB RAM memory has been left so that if we will have to load 
    something into memory in future modifications, little changes to source 
    code would be required


	At CPU startup, we use only 1-level page translation but we later switch 
	to 2-level

@@@--------------------------Kmode tables----------------------------------@@@

   1-level pgdir is located at 0x8000(32KiB) with 4096 
   entries each pointing to a 2-level page table entry

@@@------------------------------------------------------------------------@@@
   2-level page table is locate at 0x500000(5MiB) with 4096 entries each
   of size 1KiB where each of the (1KiB/4 = 256 entries) in this 1KiB points
   to a 4KiB(offset) RAM memory address amounting to (4KiBx256 = 1MiB)

@@@---------------------------Umode tables---------------------------------@@@
	
   64 process pgtables at 0x600000(6MiB) each with 4096 entries pointing to 
   the 4096 entries in the Kmode 1-level pgdir

*/

#include "globals.h"

#define   RAM_bitmap_len ((RAMSZ * PAGES_PER_BLK)/8)
unsigned char RAM_MEMORY_BITMAP[RAM_bitmap_len];	// Each bit represents a 4KiB page

unsigned int *alloc_page()
{
	
	/* First 8MiB is used by the kernel */
	for(int i = 256; i < RAM_bitmap_len; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			if( !(RAM_MEMORY_BITMAP[i] & (1 << j)))
			{
				RAM_MEMORY_BITMAP[i] |= (1 << j);
				return (unsigned int *)((i*PAGE_SIZE*8) + (j*PAGE_SIZE));
			}
		}
	}

	kprintf("No space on RAM\n");
	return 0; // No pages available
} 

void dalloc_page(unsigned int *page)
{
	unsigned int addr = (unsigned int)page;
	/* First 8MiB is used by the kernel */
	if(addr < 0x800000 ||  addr > 0x10000000)	// Must not be less than 8MiB or greater than 256MiB
		return;

	if((addr % PAGE_SIZE) != 0)
		return;

	addr =  addr / PAGE_SIZE;
	unsigned int pos = addr / 8;
	unsigned int offset = addr % 8;

	RAM_MEMORY_BITMAP[pos] &= ~(1 << offset);
}

/* For releasing all usermode virtual memory being used by a process */
void freeuvm(struct proc *proc)
{

	unsigned int *pgtable;
	for(int i = 2048; i < 4096; i++)
	{
		pgtable = (unsigned int *)((unsigned int)proc->pgdir[i] & 0xFFFFFC00);
		if((unsigned int)pgtable)
		{
			dalloc_page(pgtable);
			proc->pgdir[i] = 0;
			for(int j = 0; j < 256; j++)
			{
				if(pgtable[j] != 0)
				{
					dalloc_page((unsigned int *)((unsigned int)pgtable[j] & 0xFFFFF000));
					pgtable[j] = 0;
				}

			}

		}
	}
	proc->size = 0;
}


void clrpage(struct proc *proc, unsigned int start, int size)
{
	if(size == 0 || start < 0x80000000)
		return;

	if((size % PAGE_SIZE) != 0)		//Roundup size to the closest page size if necessary
		size = size + (PAGE_SIZE - (size % PAGE_SIZE));

	if((start % PAGE_SIZE) != 0)
		start = start + (PAGE_SIZE - (start % PAGE_SIZE));

	int plen = size/PAGE_SIZE;
	int tpos = start/0x100000; 
    int toff = (start%0x100000) / PAGE_SIZE;
    
	unsigned int *pgtable;
	while(1)
	{
		if(tpos == 4096)
			return;	

		pgtable = (unsigned int *)((unsigned int)proc->pgdir[tpos] & 0xFFFFFC00);
		if((unsigned int)pgtable)
		{
			for(int j = toff; j < 256; j++)
			{
				if(pgtable[j] != 0)
				{
					dalloc_page((unsigned int *)((unsigned int)pgtable[j] & 0xFFFFF000));
					pgtable[j] = 0;
				}

				plen--;
				if(plen == 0)
				{
					proc->size -= size;
					return;
				}
			}

		}
		toff = 0;
		tpos++;
	}
}

/* For allocatint arbitrary size of usermode virtual memory mainly for loading executables */
unsigned int allocuvm(struct proc *proc, int size)
{
	if(size == 0)
		return 0;

	if((size % PAGE_SIZE) != 0)		//Roundup size to the closest page size if necessary
		size = size + (PAGE_SIZE - (size % PAGE_SIZE));  

    int plen, n_plen;
    n_plen = plen = size/PAGE_SIZE;       // Number of pages to dynamically allocate

    unsigned int *pgtable;
    int tpos = proc->size/0x100000; 
    int toff = (proc->size%0x100000) / PAGE_SIZE;

    unsigned int start_addr = (0x80000000 + (tpos * 0x100000)) + (PAGE_SIZE *toff);  
	while(1)
	{
		if(tpos == 2048)	// This is somehow trivial because this will never occur due to the small size of the filesystem
		{
			clrpage(proc, start_addr, n_plen * PAGE_SIZE);
			return 0;  //FAILED	
		}

		if((unsigned int)proc->pgdir[2048 + tpos] == 0)	
		{
			if((unsigned int)(pgtable = alloc_page()) == 0)    // Will use only 1KiB out of the 4KiB
			{		
				clrpage(proc, start_addr, (n_plen-plen) * PAGE_SIZE);
				return 0; //FAILED
			}
			else 
			{
				proc->pgdir[2048 + tpos] = (unsigned int)pgtable | 0x31;
			}
		}
		

		pgtable = (unsigned int *)(proc->pgdir[2048 + tpos] & 0xFFFFFC00);

		for(int j = toff; j < 256; j++)
		{
			if((pgtable[j] = (unsigned int)alloc_page()) == 0)
			{
				if(j == 0)
					dalloc_page(pgtable);

				clrpage(proc, start_addr, (n_plen-plen) * PAGE_SIZE);
				return 0;  //FAILED
			}
			pgtable[j] = pgtable[j] | 0xFFE;
			proc->size += PAGE_SIZE;
			plen--;

			if(plen == 0)
				return start_addr;
		
		}
		toff = 0;
		tpos++;
		
	}
}


char buf[PAGE_SIZE];
int loaduvm(struct proc *proc, unsigned int vaddr, int fd, int f_off, int phsize)
{
	if(vaddr < 0x80000000)
		return 0;   //FAILED

    int tpos = vaddr/0x100000;
    int toff = (vaddr%0x100000) / PAGE_SIZE;

    unsigned int *pgtable, sz = 0;
    char *addr;
    klseek(fd, f_off, SEEK_SET);

	while(1)
	{
		if(tpos == 4096)
			return 0;  //FAILED

		pgtable = (unsigned int *)(proc->pgdir[tpos] & 0xFFFFFC00);
		
		for(int i = toff; i < 256; i++)
		{
			addr =(char *)((unsigned int)pgtable[i] & 0xFFFFF000);
			if((sz = kread(fd, buf, PAGE_SIZE)) < 0)
					return 0;  //FAILED

			memcpy((char *)addr, buf, sz);
			phsize -= PAGE_SIZE;

			if(phsize <= 0)
				return 1;  // SUCCESS*/
		
		}
		toff = 0;
		tpos++;

	}
}

/* This function is used to duplicate memory of parent for a child process (used in fork())*/
int copyuvm(struct proc *d_proc, struct proc *s_proc)
{
	unsigned int *d_pgtable, *s_pgtable;
	char *d_addr, *s_addr;

	int plen = s_proc->size/PAGE_SIZE;
	plen = plen - 1; //page left page stack
	int tpos = 2048;

	while(1)
	{

		if(tpos == 4096)
		{
			freeuvm(d_proc);
			return 0;
		}

		if(s_proc->pgdir[tpos])
		{	
			if((d_proc->pgdir[tpos] = alloc_page()) == 0)
			{
				freeuvm(d_proc);
				return 0; //FAILED
			}
			d_proc->pgdir[tpos] = (unsigned int)d_proc->pgdir[tpos] | 0x31;

			s_pgtable = (unsigned int *)(s_proc->pgdir[tpos] & 0xFFFFFC00);
			d_pgtable = (unsigned int *)(d_proc->pgdir[tpos] & 0xFFFFFC00);

			for(int j = 0; j < 256; j++)
			{
				if(s_pgtable[j])
				{
					if((d_pgtable[j] = alloc_page()) == 0)
					{
						freeuvm(d_proc);
						return 0; // FAILED
					}
					d_pgtable[j] = (unsigned int)d_pgtable[j] | 0xFFE;
				
					d_addr = (char *)((unsigned int)d_pgtable[j] & 0xFFFFF000);
					s_addr = (char *)((unsigned int)s_pgtable[j] & 0xFFFFF000);
					memcpy(d_addr, s_addr, PAGE_SIZE);
					plen--;

					if(plen == 0)
					{
						d_proc->size = s_proc->size;
						return 1; //SUCCESS
					}
				}
				else
				{
					d_pgtable[j] = 0;
					continue;	//using break here wont work because of the page left before the stack
				}
				
			}

		}
		else
		{
			d_proc->pgdir[tpos] = 0;
		}
		
		tpos++;
	}
	
}


/* To setup 2-level page translation in kernel initialization */
void setupvm()
{
	unsigned int *ktable = (unsigned int *)0x8000;     // At 32KiB

	for(int i = 0; i < 4096; i++)
	{
		if(i < RAMSZ)
			ktable[i] = (0x500000 + (i * 1024)) | 0x11;
		else
			ktable[i] = 0;
	}

    unsigned int *pg_table, pg_addr;
	for(int i = 0; i < RAMSZ; i++)
	{
		pg_table = (unsigned int *)(0x500000 + (i * 1024));    
        
        pg_addr = (i*0x100000 )| 0x55E;	// AP=01010101 CB=11 
		for(int j = 0; j < 256; j++)
			pg_table[j] = pg_addr + (j * PAGE_SIZE);  // Each page is 4KiB in size 
	}

  
	for(int i = 0; i < NPROCS; i++)
	{
		unsigned int *upgdir = (unsigned int *)(0x600000 + (i * 0x4000));

		for(int j = 0; j < 4096; j++)
		{
			if(j < RAMSZ)
				upgdir[j] = ktable[j];
        	else
        		upgdir[j] = 0;	
        }

	}
	switchuvm((unsigned int)ktable);

}

#endif

