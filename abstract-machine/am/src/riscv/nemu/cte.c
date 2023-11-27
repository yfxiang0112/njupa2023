#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  __am_get_cur_as(c);

  if (user_handler) {
    Event ev = {0};

    if (c->mcause >=0 && c->mcause < 20) {
      ev.event = EVENT_SYSCALL;
    }

    else if (c->mcause == -1) {
      ev.event = EVENT_YIELD;
    }

    else if (c->mcause == 0x80000007) {
      ev.event = EVENT_IRQ_TIMER;
    }

    else {
      printf("Unkown mcause code: %d\n", c->mcause);
      ev.event = EVENT_ERROR; 
    }
    
    c = user_handler(ev, c);
    assert(c != NULL);
  }

  __am_switch(c);
  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));
  asm volatile("csrw mscratch, zero");

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context *ksp = (Context*)((uintptr_t)(kstack.end) - CONTEXT_SIZE);

  ksp -> gpr[2] = (uintptr_t)ksp;
  ksp -> gpr[10] = (uintptr_t)arg;
  ksp -> mepc = (uintptr_t)entry;
  ksp -> mstatus = 0x88;

  ksp -> next_priv = 0;
  ksp -> pdir = NULL;

  return ksp;
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
