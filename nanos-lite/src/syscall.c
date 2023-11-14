#include <common.h>
#include "syscall.h"
#include <sys/time.h>

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;


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
      break;

    case SYS_read:
      c->GPRx = fs_read(a[1], (void*)a[2], a[3]);
      break;

    case SYS_write: 
      c->GPRx = fs_write(a[1], (void*)a[2], a[3]);
      break;

    case SYS_close:
      c->GPRx = fs_close(a[1]);
      break;

    case SYS_lseek:
      c->GPRx = fs_lseek(a[1], a[2], a[3]);
      break;

    case SYS_brk:
      //TODO: 
      for (uint32_t i=0; i<a[2]; i++) {
        *(uint32_t*)(a[1] + i) = 0;
      }
      c->GPRx = 0;
      break;

    case SYS_gettimeofday:
      uint32_t tick = io_read(AM_TIMER_UPTIME).us;
      ((struct timeval *)a[1])->tv_usec = tick;
      ((struct timeval *)a[1])->tv_sec = tick / 1000;
      c->GPRx = 0;
      break;
      

    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  #ifdef ENABLE_STRACE
  if (a[0]==SYS_read || a[0]==SYS_write || a[0]==SYS_close || a[0]==SYS_lseek) {
    printf("[STRACE]:         file operation at %d\n", a[1]);
  }
  if (a[0]==SYS_open) {
    printf("[STRACE]:         file operation at %d\n", c->GPRx);
  }
  #endif

}
