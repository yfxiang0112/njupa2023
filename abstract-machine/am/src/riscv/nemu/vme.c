#include <am.h>
#include <nemu.h>
#include <riscv/riscv.h>
#include <klib.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

int cnt=0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }

  set_satp(kas.ptr);
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->pdir != NULL) {
    set_satp(c->pdir);
  }
}

void map(AddrSpace *as, void *va, void *pa, int prot) {
  uintptr_t pdir = (uintptr_t)(as->ptr) >> 12;

  uintptr_t vpn1, vpn0;
  uintptr_t ppn1, ppn0;
  uintptr_t pte;
  uintptr_t pte1_addr, pte0_addr;
  uintptr_t v=1, r=1<<1, w=1<<2, x=1<<3;

  vpn1 = (uintptr_t) va & 0xffc00000;
  vpn0 = (uintptr_t) va & 0x003ff000;
  ppn1 = (uintptr_t) pa & 0xffc00000;
  ppn0 = (uintptr_t) pa & 0x003ff000;

  pte1_addr = pdir * PGSIZE + (vpn1>>22) * PTESIZE;

  if (*(uintptr_t*)pte1_addr == 0) {
    pte0_addr = (uintptr_t) pgalloc_usr(PGSIZE);
    *(uintptr_t*)pte1_addr = ((pte0_addr & 0xfffff000) >>2) | v;

    if (pte1_addr == 0x821387fc) { printf("pte=%x\n", ((pte0_addr & 0xfffff000) >>2) | v);}

  } else {
    uintptr_t pte_ppn = ((*(uintptr_t*)pte1_addr) & 0xfffffc00) >> 10;
    pte0_addr = pte_ppn * PGSIZE + (vpn0>>12) * PTESIZE; 
  }
  //printf("pte0_addr = %x, va=%x\n", pte0_addr, va);

  pte = (ppn1>>2) | (ppn0>>2) | x | w | r | v;
  *(uintptr_t*)pte0_addr = pte;

  if ((uintptr_t)va < 0x4000dc7c && (uintptr_t)va+4096 > 0x4000dc7c ) {
    printf("@map: va = %x, pa = %x\n", va, pa);
  }
  if (pte0_addr == 0x821387fc) { printf("pte=%x\n", pte);}

}

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
  Context *cp = (Context*)((uintptr_t)(kstack.end) - CONTEXT_SIZE);

  cp -> gpr[2] = (uintptr_t)cp;
  //ksp -> gpr[10] = (uintptr_t)arg;
  cp -> mepc = (uintptr_t)entry;

  cp -> pdir = as -> ptr;

  return cp;
}
