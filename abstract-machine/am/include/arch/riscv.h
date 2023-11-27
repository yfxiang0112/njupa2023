#ifndef ARCH_H__
#define ARCH_H__

#ifndef __riscv_e
#define NR_REGS 32
#else
#define NR_REGS 16
#endif

struct Context {
  uintptr_t gpr[NR_REGS], mcause, mstatus, mepc;

  uintptr_t next_priv;
  void *pdir;
};

#ifdef __riscv_e
#define GPR1 gpr[15] // a5
#else
#define GPR1 gpr[17] // a7
#endif

#define GPR2 gpr[10]
#define GPR3 gpr[11]
#define GPR4 gpr[12]
#define GPRx gpr[10]


#if __riscv_xlen == 32
#define LOAD  "lw"
#define STORE "sw"
#define XLEN  4
#else
#define LOAD  "ld"
#define STORE "sd"
#define XLEN  8
#endif

#define CONTEXT_SIZE  ((NR_REGS + 3 + 1 + 1) * XLEN)


/* temporarily used by asm contex backup */
/*
#define REGS_LO16(f) \
      f( 1)       f( 3) f( 4) f( 5) f( 6) f( 7) f( 8) f( 9) \
f(10) f(11) f(12) f(13) f(14) f(15)
#ifndef __riscv_e
#define REGS_HI16(f) \
                                    f(16) f(17) f(18) f(19) \
f(20) f(21) f(22) f(23) f(24) f(25) f(26) f(27) f(28) f(29) \
f(30) f(31)
#else
#define REGS_HI16(f)
#endif
#define REGS(f) REGS_LO16(f) REGS_HI16(f)

#define str_temp(s) #s
#define str(s) str_temp(s)
#define MAP(c, f) c(f)

#define PUSH(i) STORE x##i (i * XLEN)(%ksp);
#define PUSH_STR(i) str(PUSH(i))

*/

#endif
