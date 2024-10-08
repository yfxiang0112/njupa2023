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

#include "../local-include/reg.h"
#include <isa.h>
#include <memory/vaddr.h>
#include <memory/paddr.h>
#include <memory/host.h>

int isa_mmu_check(vaddr_t vaddr, int len, int type) {
  if (csr(SR_SATP)==0) { return MMU_DIRECT; }
  if (csr(SR_SATP) >> 31 == 1) { return MMU_TRANSLATE; }
  return MMU_FAIL;
}

paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  paddr_t vpn1, vpn0, voff;
  paddr_t ppn1, ppn0, paddr;
  vpn1 = vaddr & 0xffc00000;
  vpn0 = vaddr & 0x003ff000;
  voff = vaddr & 0x00000fff;

  paddr_t satp_ppn = csr(SR_SATP) & 0x003fffff;
  paddr_t pte_addr = satp_ppn * 4096 + (vpn1>>22) * 4;

  word_t pte = host_read(guest_to_host(pte_addr), 4);
  if(!(pte&1)) {
    //nemu_state.state = NEMU_ABORT;
    //return 0x80000000;
    panic("invalid PTE: vaddr=0x%x, pte_addr=0x%x, pte=0x%x\n", vaddr, pte_addr, pte);
  } 

  if ((pte&0x2)==0 && (pte&0x4)==0 && (pte&0x8)==0) {
    
    paddr_t pte0_addr = ((pte & 0xfffffc00)>>10) * 4096 + (vpn0>>12) * 4;
    pte = host_read(guest_to_host(pte0_addr), 4);

    if(!(pte&1)) {
      panic("invalid PTE: vaddr=0x%x, pte_addr=0x%x, pte=0x%x\n", vaddr, pte_addr, pte);
    } 

    ppn1 = pte   & 0xfff00000;
    ppn0 = pte   & 0x000ffc00;
    paddr = (ppn1<<2) | (ppn0<<2) | voff;
  }
  else {
    ppn1 = pte   & 0xfff00000;
    paddr = (ppn1<<2) | (vpn0) | voff;
  }

  //printf("paddr=%x, vaddr=%x\n", paddr, vaddr);
  /*
  if (vaddr>0x70000000 && vaddr<0x80000000) {
    printf("vaddr=%x, paddr=%x\n", vaddr, paddr);
  }
  */

  // NOTE: for tmp test of equiv mapping
  // assert(paddr == vaddr);

  return paddr;
}
