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

#include <isa.h>
#include "../local-include/reg.h"

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
#ifdef CONFIG_ETRACE
  printf("%s type %2d, at addr 0x%08x, jump to 0x%08x\n",
         ANSI_FMT("[ETRACE]:", ANSI_FG_YELLOW), NO, epc, csr(SR_MTVEC));
#endif

  csr(SR_MEPC) = epc;
  csr(SR_MCAUSE) = NO;

  csr(SR_MSTATUS) = csr(SR_MSTATUS) & 0xffffff7f;
  csr(SR_MSTATUS) = csr(SR_MSTATUS) | ((csr(SR_MSTATUS)&0b1000)<<4);
  csr(SR_MSTATUS) = csr(SR_MSTATUS) & 0xfffffff7;

  return csr(SR_MTVEC);
}

word_t isa_query_intr() {
  if (cpu.intr && (csr(SR_MSTATUS)&0b1000)) {
    cpu.intr = false;
    return IRQ_TIMER;
  }
  return INTR_EMPTY;
}
