#include <common.h>
#include <proc.h>

static Context* do_event(Event e, Context* c) {
  printf("test @irq.c:5\n");
  switch (e.event) {
    case EVENT_YIELD: 
    printf("test @irq.c:7\n");
      return schedule(c);

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
