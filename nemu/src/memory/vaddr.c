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
#include <memory/paddr.h>

word_t vaddr_ifetch(vaddr_t addr, int len) {
  paddr_t paddr = addr;
  int check = isa_mmu_check(addr, len, 0);
  assert(check != MMU_FAIL);

  if (check == MMU_TRANSLATE) {
    paddr = isa_mmu_translate(addr, len, 0);

    // NOTE: tmp test for equiv mapping
    assert(paddr == addr);
  }
  return paddr_read(paddr, len, false);
}

word_t vaddr_read(vaddr_t addr, int len, bool is_gst) {
  paddr_t paddr = addr;
  int check = isa_mmu_check(addr, len, 0);
  assert(check != MMU_FAIL);

  if (check == MMU_TRANSLATE) {
    paddr = isa_mmu_translate(addr, len, 0);

    // NOTE: tmp test for equiv mapping
    assert(paddr == addr);
  }
  return paddr_read(paddr, len, is_gst);
}

void vaddr_write(vaddr_t addr, int len, word_t data, bool is_gst) {
  paddr_t paddr = addr;
  int check = isa_mmu_check(addr, len, 0);
  assert(check != MMU_FAIL);

  if (check == MMU_TRANSLATE) {
    paddr = isa_mmu_translate(addr, len, 0);

    // NOTE: tmp test for equiv mapping
    assert(paddr == addr);
  }

  paddr_write(paddr, len, data, is_gst);
}
