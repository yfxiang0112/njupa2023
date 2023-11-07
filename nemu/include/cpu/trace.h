#ifndef __CPU_TRACE_H
#define __CPU_TRACE_H

#include <cpu/decode.h> 

#define TRACE_STR(x) ANSI_FMT(x, ANSI_FG_YELLOW)

void rec_itrace(Decode *_this);
void ring_itrace();

void mtrace(paddr_t addr, int len, word_t data, char* type, bool is_gst);

void dtrace(paddr_t addr, int len, word_t data, const char* name, char* type, bool is_gst);

#endif
