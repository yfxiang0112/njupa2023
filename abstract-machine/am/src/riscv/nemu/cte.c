#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

#define str_temp(s) #s
#define str(s) str_temp(s)

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

  asm volatile ("addi %0, %0, %1" : "+r"(ksp) : "i"(-CONTEXT_SIZE));

  //char asm_buf[64];

  printf("%s\n", str(i));
  /*
  for (int i=1; i<NR_REGS; i++) {
    //sprintf(asm_buf, "sw x%d, %d(\%[ksp]);", i, i);
    asm volatile (
      "sw x" str(i) ", %i(%[ksp])" 
      : [ksp]"+r"(ksp)
      : [i]"i"(i*XLEN)
    );
  }

  //printf("%s\n", STR( MAP(REGS, KPUSH) ));
    
  */
  /*
  asm volatile (
    //"sw x1, %[i1](%[ksp]);"
    //"sw x2, %[i2](%[ksp]);"
    STR(MAP(REGS, KPUSH))

    : [ksp]"+r"(ksp)
    //: [i1]"i"(1*XLEN), [i2]"i"(2*XLEN)
    : MAP(REGS, IMM)
  );
  */

  for (int i=0; i<36; i++) {
    printf("%x\n", ((uint32_t*)ksp)[i]);
  }

  

  return NULL;
  //return (Context*)ksp;
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
