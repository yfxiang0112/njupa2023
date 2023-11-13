#include <common.h>
#include "syscall.h"

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
    
    case SYS_open:
      c->GPRx = fs_open((char*)c->GPR2, c->GPR3, c->GPR4);
      break;

    case SYS_read:
      if (c->GPR2 == 0) {
        //TODO:
        break;
      }
      c->GPRx = fs_read(c->GPR2, (void*)c->GPR3, c->GPR4);
      break;

    case SYS_write: 
      if (c->GPR2 == 1 || c->GPR2 == 2){
        for (int i=0; i<c->GPR4; i++) {
          putch(*((char*)c->GPR3 + i));
        }
        c->GPRx = c->GPR4;
        break;
      }
      c->GPRx = fs_write(c->GPR2, (void*)c->GPR3, c->GPR4);
      break;

    case SYS_close:
      c->GPRx = fs_close(c->GPR2);
      break;

    case SYS_lseek:
      c->GPRx = fs_lseek(c->GPR2, c->GPR3, c->GPR4);
      break;

    case SYS_brk:
      
      c->GPRx = 0;
      break;
      

    default: panic("Unhandled syscall ID = %d", a[0]);
  }

}
