#ifndef __CPU_TRACE_H
#define __CPU_TRACE_H

#include <cpu/decode.h> 

void rec_itrace(Decode *_this);
void ring_itrace();

void mtrace_r(paddr_t addr, int len, word_t data);
void mtrace_w(paddr_t addr, int len, word_t data);

#endif
