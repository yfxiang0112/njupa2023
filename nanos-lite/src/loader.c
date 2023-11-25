#include <proc.h>
#include <memory.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr elf;
  Elf_Phdr ph;
  size_t fd;
  char *load_va, *load_pg=0;
  uintptr_t pa_start;

  fd = fs_open(filename, 0, 0);
  if(fd==2) { return -2; }

  fs_read(fd, &elf, sizeof(Elf_Ehdr));

  assert(elf.e_ident[0] == 0x7f);
  assert(elf.e_ident[1] == 0x45);
  assert(elf.e_ident[2] == 0x4c);
  assert(elf.e_ident[3] == 0x46);

  for (int i=0; i<elf.e_phnum; i++) {
    fs_lseek(fd, elf.e_phoff + i*elf.e_phentsize, 0);
    fs_read(fd, &ph, elf.e_phentsize);

    if (ph.p_type == 1) {
      load_va = (char*) ( ph.p_vaddr & 0xfffff000);
      pa_start = 0;
      fs_lseek(fd, ph.p_offset, 0);

      if (pcb) {
        //printf("&(pcb->as) = %x\n", &(pcb->as));
        printf("start=%x, fileend=%x, memend=%x\n", ph.p_vaddr, ph.p_vaddr+ph.p_filesz, ph.p_vaddr+ph.p_memsz);

        while ((uintptr_t)load_va <= ph.p_vaddr+ph.p_memsz) {
          load_pg = new_page(1);
          assert(&(pcb->as));
          map(&(pcb->as), load_va, load_pg, 0b111);

          fs_read(fd, load_pg + (ph.p_vaddr&0xfff), PGSIZE);

          /*
          if ((uintptr_t)load_va<0x40004a68 && (uintptr_t)load_va+PGSIZE>0x40004a68) {
            printf("orig paddr=%x\n", 0x40004a68 - (uintptr_t)load_va + (uintptr_t)load_pg);
            printf("orig val = %08x\n", *(uint32_t*)(0x40004a68 - (uintptr_t)load_va + (uintptr_t)load_pg));
            for (int i=-16; i<16; i+=4) {
              printf("%08x\n", *(uint32_t*)(0x40004a68 - (uintptr_t)load_va + (uintptr_t)load_pg + i));
            }
          }
          */

          load_va += PGSIZE;
          if (!pa_start) pa_start = (uintptr_t)load_pg;
        }
        printf("pgend=%x, pastart=%x, fileend=%x, memend=%x\n", (uintptr_t)load_pg+PGSIZE, pa_start, pa_start+ph.p_filesz, pa_start+ph.p_memsz);
        //printf("set_len=%x\n", ((uintptr_t)load_pg-pa_start-ph.p_filesz)%PGSIZE);
        // NOTE: the first mmbrk page might be not zeroed out
        memset((char*)(pa_start+ph.p_filesz+1), 0, ph.p_memsz-ph.p_filesz);//(uintptr_t)load_pg+PGSIZE-pa_start-ph.p_filesz);
        pcb->max_brk = (uintptr_t)load_va;
        //pcb->max_brk = ph.p_vaddr + ph.p_memsz;
      } else {
        fs_read(fd, load_va, ph.p_filesz);
        memset((char*)(ph.p_vaddr+ph.p_filesz), 0, ph.p_memsz-ph.p_filesz);
      }
    }
  }
  
  fs_close(fd);
  printf("entry=%x\n", elf.e_entry);
  return elf.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  fb_write(NULL, 0, io_read(AM_GPU_CONFIG).width*io_read(AM_GPU_CONFIG).width);
  uintptr_t entry = loader(pcb, filename);

  if (!entry) {
    return;
  }

  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

