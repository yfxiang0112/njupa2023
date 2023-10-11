#ifndef __CPU_TRACE_H
#define __CPU_TRACE_H

#include <cpu/decode.h> 

void rec_itrace(Decode *_this);
void quit_itrace();

void mtrace_r();
void mtrace_w();

#endif
