#ifndef ELF_H
#define ELF_H

#define EI_NIDENT 16

/* Values for elf32_phdr type */
#define ELF_PROG_LOAD           1

/* Flag bits for Proghdr flags */
#define ELF_PROG_FLAG_EXEC      1
#define ELF_PROG_FLAG_WRITE     2
#define ELF_PROG_FLAG_READ      4

/* Only these two headers will help us to load programs due to the simple structure of the executables */
struct elf32_ehdr
{
	unsigned char   e_ident[EI_NIDENT];
	unsigned short  e_type;
	unsigned short  e_machine;
	unsigned int    e_version;
	unsigned int    e_entry;
	unsigned int    e_phoff;
	unsigned int    e_shoff;
	unsigned int    e_flags;
	unsigned short  e_ehsize;
	unsigned short  e_phentsize;
	unsigned short  e_phnum;
	unsigned short  e_shentsize;
	unsigned short  e_shnum;
	unsigned short  e_shstrndex;   
};

struct elf32_phdr
{
	unsigned int p_type;
	unsigned int p_offset;
	unsigned int p_vaddr;
	unsigned int p_paddr;
	unsigned int p_filesz;
	unsigned int p_memsz;
	unsigned int p_flags;
	unsigned int p_align;

};

#endif