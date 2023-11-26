#include <common.h>
#include <proc.h>

static Context* do_event(Event e, Context* c) {
  switch (e.event) {
    case EVENT_YIELD: 
      return schedule(c);

    case EVENT_SYSCALL:
      do_syscall(c);
      break;

    case EVENT_IRQ_TIMER:
      //Log("Nanos-lite received irq-timer event");
      printf("timer interrupt\n");
      return schedule(c);
      break;

    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
