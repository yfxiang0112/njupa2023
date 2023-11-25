#include <memory.h>
#include <proc.h>

static void *pf;
extern PCB *current;

void* new_page(size_t nr_page) {
  void* ret = pf;
  pf += nr_page * PGSIZE;

  if ((uintptr_t)ret < 0x82173c7c && (uintptr_t)pf > 0x82173c7c) {
    printf("test @newpg : nrpg=%d\n", nr_page);
  }

  return ret;
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
  uintptr_t curbrk = current->max_brk;
  //printf("brk=%x, curbrk=%x\n",brk, curbrk);
  if (brk > curbrk) {
    while (brk > curbrk) {
      map(&(current->as), (char*)curbrk, pg_alloc(PGSIZE), 0b111);
      curbrk += PGSIZE;
      printf("brk=%x, curbrk=%x\n",brk, curbrk);
    }
    current->max_brk = curbrk;
  }
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %x", (uintptr_t)pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
