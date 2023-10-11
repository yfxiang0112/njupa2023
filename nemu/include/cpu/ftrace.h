#ifndef __CPU_FTRACE_H__
#define __CPU_FTRACE_H__

#include <common.h>
#include <elf.h>

typedef struct Funct {
	vaddr_t addr;
	word_t size;
	char name[65];
} Funct;


void init_ftrace(const char *elf_file);
void rec_ftrace(vaddr_t addr, vaddr_t pc);

#endif
