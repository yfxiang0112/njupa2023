#ifndef RISCV_H__
#define RISCV_H__

#include <stdint.h>
#include <stdio.h>

static inline uint8_t  inb(uintptr_t addr) { return *(volatile uint8_t  *)addr; }
static inline uint16_t inw(uintptr_t addr) { return *(volatile uint16_t *)addr; }
static inline uint32_t inl(uintptr_t addr) { return *(volatile uint32_t *)addr; }

static inline void outb(uintptr_t addr, uint8_t  data) { *(volatile uint8_t  *)addr = data; }
static inline void outw(uintptr_t addr, uint16_t data) { *(volatile uint16_t *)addr = data; }
static inline void outl(uintptr_t addr, uint32_t data) { *(volatile uint32_t *)addr = data; }

#define PTE_V 0x01
#define PTE_R 0x02
#define PTE_W 0x04
#define PTE_X 0x08
#define PTE_U 0x10
#define PTE_A 0x40
#define PTE_D 0x80

enum { MODE_U, MODE_S, MODE_M = 3 };
#define MSTATUS_MXR  (1 << 19)
#define MSTATUS_SUM  (1 << 18)

#if __riscv_xlen == 64
#define MSTATUS_SXL  (2ull << 34)
#define MSTATUS_UXL  (2ull << 32)
#else
#define MSTATUS_SXL  0
#define MSTATUS_UXL  0
#endif

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

#if __riscv_xlen == 32
#define LOAD  "lw"
#define STORE "sw"
#define XLEN  4
#else
#define LOAD  "ld"
#define STORE "sd"
#define XLEN  8
#endif

#define str_temp(s) #s
#define str(s) str_temp(s)
#define MAP(c, f) c(f)

#define PUSH(i) STORE x##i (i * XLEN)(%ksp);
#define PUSH_STR(i) str(PUSH(i))

#define CONTEXT_SIZE  ((NR_REGS + 3 + 1) * XLEN)

#endif
