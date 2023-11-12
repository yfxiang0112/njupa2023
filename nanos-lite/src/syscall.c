#include <common.h>
#include "syscall.h"

extern char end;

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  #ifdef ENABLE_STRACE
  printf("[STRACE]: syscall ID = %d at pc = 0x%x\n", a[0], c->mepc);
  #endif

  switch (a[0]) {
    case SYS_exit:  
      halt(c->GPR2);  
      c->GPRx = 0;
      break;

    case SYS_yield: 
      yield();  
      c->GPRx = 0;
      break;
    
    case SYS_write: 
      if (c->GPR2 == 1 || c->GPR2 == 2){
        for (int i=0; i<c->GPR4; i++) {
          putch(*((char*)c->GPR3 + i));
        }
      }
      c->GPRx = c->GPR4;
      break;

    case SYS_brk:
      putch('0'+end);
      c->GPRx = 0;
      break;
      

    default: panic("Unhandled syscall ID = %d", a[0]);
  }

}
