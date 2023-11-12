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
  char *load_ptr;

  ramdisk_read(&elf, 0, sizeof(Elf_Ehdr));

  assert(elf.e_ident[0] == 0x7f);
  assert(elf.e_ident[1] == 0x45);
  assert(elf.e_ident[2] == 0x4c);
  assert(elf.e_ident[3] == 0x46);

  for (int i=0; i<elf.e_phnum; i++) {
    fs_open("", 0, 0);
    ramdisk_read(&ph, elf.e_phoff + i*elf.e_phentsize, elf.e_phentsize);

    if (ph.p_type == 1) {
      load_ptr = (char*) ph.p_vaddr;
      ramdisk_read(load_ptr, ph.p_offset, ph.p_filesz);
      memset(load_ptr+ph.p_filesz, ph.p_memsz-ph.p_filesz, 0);
    }
  }
  
  return elf.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

