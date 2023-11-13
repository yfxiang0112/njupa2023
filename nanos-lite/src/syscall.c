#include <common.h>
#include "syscall.h"

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  int fd = -1;

  #ifdef ENABLE_STRACE
  printf("[STRACE]: syscall ID = %d at pc = 0x%x\n", a[0], c->mepc);
  #endif

  switch (a[0]) {
    case SYS_exit:  
      halt(a[1]);  
      c->GPRx = 0;
      break;

    case SYS_yield: 
      yield();  
      c->GPRx = 0;
      break;
    
    case SYS_open:
      c->GPRx = fs_open((char*)a[1], a[2], a[3]);
      fd = c->GPRx;
      break;

    case SYS_read:
      if (a[1] == 0) {
        //TODO:
        break;
      }
      c->GPRx = fs_read(a[1], (void*)a[2], a[3]);
      fd = a[1];
      break;

    case SYS_write: 
      if (a[1] == 1 || a[1] == 2){
        for (int i=0; i<a[3]; i++) {
          putch(*((char*)a[2] + i));
        }
        c->GPRx = a[3];
        break;
      }
      c->GPRx = fs_write(a[1], (void*)a[2], a[3]);
      fd = a[1];
      break;

    case SYS_close:
      c->GPRx = fs_close(a[1]);
      fd = a[1];
      break;

    case SYS_lseek:
      c->GPRx = fs_lseek(a[1], a[2], a[3]);
      fd = a[1];
      break;

    case SYS_brk:
      //TODO: 
      c->GPRx = 0;
      break;
      

    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  #ifdef ENABLE_STRACE
  if (fd >= 0) {
    printf("[STRACE]:         file operation at %d\n", fd);
  }
  #endif

}
