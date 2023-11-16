#include <common.h>

static Context* do_event(Event e, Context* c) {
  //printf("contex=%x\n", &c);
  switch (e.event) {
    case EVENT_YIELD: 
      printf("nanos-lite triggered yield\n");
      break;

    case EVENT_SYSCALL:
      do_syscall(c);
      break;

    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
