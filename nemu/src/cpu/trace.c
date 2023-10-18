#include <cpu/trace.h>
#include <cpu/decode.h>

#define I_TRACE_BUF_LEN  16

#ifdef CONFIG_ITRACE_QUIT
char iringbuf[16][128];
int ringidx = 0;
#endif


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

void mtrace(paddr_t addr, int len, word_t data, char* type) {
	IFDEF(CONFIG_MTRACE, if (CONFIG_MTRACE) printf("MTRACE: %5s at %x(%d) = 0x%x\n", type, addr, len, data);)
}

void mtrace_w(paddr_t addr, int len, word_t data) {
	IFDEF(CONFIG_MTRACE, if (CONFIG_MTRACE) printf("MTRACE: write at %x(%d) = 0x%x\n", addr, len, data);)
}
