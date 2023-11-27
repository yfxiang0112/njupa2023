/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#ifndef __RISCV_REG_H__
#define __RISCV_REG_H__

#include <common.h>

// GPR ///////////////////////

static inline int check_reg_idx(int idx) {
  IFDEF(CONFIG_RT_CHECK, assert(idx >= 0 && idx < MUXDEF(CONFIG_RVE, 16, 32)));
  return idx;
}

#define gpr(idx) (cpu.gpr[check_reg_idx(idx)])

static inline const char* reg_name(int idx) {
  extern const char* regs[];
  return regs[check_reg_idx(idx)];
}


// CSR ///////////////////////

#define SR_MTVEC     0x305
#define SR_MEPC      0x341
#define SR_MSTATUS   0x300
#define SR_MCAUSE    0x342
#define SR_SATP      0x180
#define SR_MSCRATCH  0x340

static inline int check_csr_idx(int idx) {
  if (idx >= 0 && idx < 4) { return idx; }
  if (idx == SR_MTVEC)      { return 0; }
  if (idx == SR_MEPC)       { return 1; }
  if (idx == SR_MSTATUS)    { return 2; }
  if (idx == SR_MCAUSE)     { return 3; }
  if (idx == SR_SATP)       { return 4; }
  if (idx == SR_MSCRATCH)   { return 5; }
  IFDEF(CONFIG_RT_CHECK, assert(0));
}

#define csr(idx) (cpu.csr[check_csr_idx(idx)])

static inline const char* csr_name(int idx) {
  extern const char* csrs[];
  return csrs[check_csr_idx(idx)];
}

#endif
