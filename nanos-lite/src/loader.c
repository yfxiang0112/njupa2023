#include <proc.h>
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
  char *load_ptr;

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
      load_ptr = (char*) ph.p_vaddr;
      fs_lseek(fd, ph.p_offset, 0);
      fs_read(fd, load_ptr, ph.p_filesz);
      memset(load_ptr+ph.p_filesz, 0, ph.p_memsz-ph.p_filesz);
    }
  }
  
  fs_close(fd);
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

