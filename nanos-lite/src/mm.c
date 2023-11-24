#include <memory.h>

static void *pf;

void* new_page(size_t nr_page) {
  pf += nr_page * PGSIZE;
  printf("pf=%x\n", (uintptr_t)pf);
  return pf;
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  assert(n%PGSIZE == 0);
  void* new_pg = new_page(n/PGSIZE);
  memset(new_pg, 0, n);
  return new_pg;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  //return 0;
  assert(0);
  return -1;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %x", (uintptr_t)pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
