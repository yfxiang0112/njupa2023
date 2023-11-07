#include <common.h>
#include "syscall.h"

void do_syscall(Context *c) {
  char ev_str[10] = "";
  uintptr_t a[4];
  a[0] = c->GPR1;


  switch (a[0]) {
    case SYS_exit:  halt(0);  strcpy(ev_str,"exit");  break;
    case SYS_yield: yield();  strcpy(ev_str,"Yield"); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  printf("[STRACE]: syscall [ %d ] event %s\n", a[0], ev_str);
}
