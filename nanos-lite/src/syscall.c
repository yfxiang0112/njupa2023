#include <common.h>
#include "syscall.h"

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  printf("[STRACE]: syscall ID =  %d at pc = 0x%x\n", a[0], c->mepc);

  switch (a[0]) {
    case SYS_exit:  halt(0);  break;
    case SYS_yield: yield();  break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

}
