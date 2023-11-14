#include <cpu/trace.h>
#include <cpu/decode.h>
#include <device/map.h> 

#define I_TRACE_BUF_LEN  16

#ifdef CONFIG_ITRACE_QUIT
char iringbuf[16][128];
int ringidx = 0;
#endif

extern CPU_state cpu;

void rec_itrace(Decode *_this) {
#ifdef CONFIG_ITRACE_QUIT
	if (ITRACE_COND) {
		strncpy(iringbuf[ringidx], _this->logbuf, 128);
		ringidx = (ringidx +1) % I_TRACE_BUF_LEN;
	}
#endif
}

void ring_itrace() {
#ifdef CONFIG_ITRACE_COND
			if (ITRACE_COND) {
				log_write("\nlast instructions be executed:\n");
				printf(ANSI_FMT("\nlast instructions be executed:\n", ANSI_FG_RED));

				for (int i=ringidx+1; i<I_TRACE_BUF_LEN; i++) {
					log_write("%s\n",iringbuf[i]);
					printf("%s\n", iringbuf[i]);
				}
				for (int i=0; i<ringidx-1; i++) {
					log_write("%s\n",iringbuf[i]);
					printf("%s\n", iringbuf[i]);
				}

				printf(ANSI_FMT("%s\n", ANSI_FG_RED), iringbuf[ringidx-1]);
				log_write("%s\n", iringbuf[ringidx-1]);
			}
#endif
}

void mtrace(paddr_t addr, int len, word_t data, char* type, bool is_gst) {
#ifdef CONFIG_MTRACE
  //if (is_gst) { printf("%s %5s at %x(%d) = 0x%x\n", TRACE_STR("[MTRACE]:"), type, addr, len, data); }
  //else { IFDEF(CONFIG_MTRACE_ALL, printf("%s %5s at %x(%d) = 0x%x (NEMU)\n", TRACE_STR("[MTRACE]:"), type, addr, len, data)); }
  if (strcmp(type, "write")==0 && addr >= 0x83000000 && addr <= 0x8300765c) {
    printf(ANSI_FMT("segment 1 at pc=0x%x\n", ANSI_FG_RED), cpu.pc);
  }
  if (strcmp(type, "write")==0 && addr >= 0x83008660 && addr <= (0x83008660 + 0x008e8)) { 
    printf(ANSI_FMT("segment 2\n", ANSI_FG_RED)); 
  }
#endif
}

void dtrace(paddr_t addr, int len, word_t data, const char* name, char* type, bool is_gst) {
#ifdef CONFIG_DTRACE
  if (is_gst) { printf("%s %5s %s at %x(%d) = 0x%x\n", TRACE_STR("[MTRACE]:"), type, name, addr, len, data); }
  else { IFDEF(CONFIG_DTRACE_ALL, printf("%s %5s %s at %x(%d) = 0x%x (NEMU)\n", TRACE_STR("[MTRACE]:"), type, name, addr, len, data)); }
#endif
}
