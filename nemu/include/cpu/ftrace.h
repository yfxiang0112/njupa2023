#ifndef __CPU_FTRACE_H__
#define __CPU_FTRACE_H__

#include <common.h>

typedef struct Funct {
	vaddr_t addr;
	word_t size;
	char name[65];
} Funct;

extern Funct **funct_tab;

void recr_ftrace();

#endif
