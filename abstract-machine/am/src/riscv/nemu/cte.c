#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

#if __riscv_xlen == 32
#define LOAD  lw
#define STORE sw
#define XLEN  4
#else
#define LOAD  ld
#define STORE sd
#define XLEN  8
#endif

#define REGS_LO16(f) \
      f( 1)       f( 3) f( 4) f( 5) f( 6) f( 7) f( 8) f( 9) \
f(10) f(11) f(12) f(13) f(14) f(15)
#ifndef __riscv_e
#define REGS_HI16(f) \
                                    f(16) f(17) f(18) f(19) \
f(20) f(21) f(22) f(23) f(24) f(25) f(26) f(27) f(28) f(29) \
f(30) f(31)
#define NR_REGS 32
#else
#define REGS_HI16(f)
#define NR_REGS 16
#endif

#define CONTEXT_SIZE  ((NR_REGS + 3 + 1) * XLEN)

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  if (user_handler) {
    Event ev = {0};

    if (c->mcause >=0 && c->mcause < 20) {
      ev.event = EVENT_SYSCALL;
    }

    else if (c->mcause == -1) {
      ev.event = EVENT_YIELD;
    }

    else {
      printf("Unkown mcause code: %d\n", c->mcause);
      ev.event = EVENT_ERROR; 
    }
    
    c = user_handler(ev, c);
    assert(c != NULL);
  }

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  //asm volatile("csrw mtvec, %0" : : "r"(entry));
  uintptr_t ksp = (uintptr_t)(kstack.end);


  printf("%x\n", ksp);
  asm volatile ("addi %0, %0, %1" : "+r"(ksp) : "i"(-CONTEXT_SIZE));

  printf("%x\n", ksp);

  

  return NULL;
}

void yield() {
#ifdef __riscv_e
  asm volatile("li a5, -1; ecall");
#else
  asm volatile("li a7, -1; ecall");
#endif
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
