#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr elf;
  Elf_Phdr ph;
  char *load_ptr, *ret_ptr=NULL;

  ramdisk_read(&elf, 0, sizeof(Elf_Ehdr));
  printf("0x%x, off=0x%x, num=0x%x\n", elf, elf.e_phoff, elf.e_phnum);
  printf("0x%x\n", elf.e_ident[0]);

  for (int i=0; i<elf.e_phnum; i++) {
    ramdisk_read(&ph, elf.e_phoff + i*elf.e_phentsize, elf.e_phentsize);
    printf("%d, type=0x%x, addr=0x%x, fsz=0x%x, msz=0x%x\n",
           i, ph.p_type, ph.p_vaddr, ph.p_filesz, ph.p_memsz);
    if (ph.p_type == 1) {
      load_ptr = (char*) ph.p_vaddr;
      if (!ret_ptr) { ret_ptr = load_ptr; }
      printf("ptr = 0x%x\n", load_ptr);
      ramdisk_read(load_ptr, ph.p_offset, ph.p_filesz);
      memset(load_ptr+ph.p_filesz, ph.p_memsz-ph.p_filesz, 0);
    }
  }
  
  //TODO();
  return (uintptr_t)ret_ptr;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

