#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, invalid_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

#define NR_FILES sizeof(file_table)/sizeof(Finfo)

void init_fs() {
  // TODO: initialize the size of /dev/fb
}


int fs_open(const char *pathname, int flags, int mode) {
  for (int idx=0; idx<NR_FILES; idx++) {
    if (strcmp(file_table[idx].name , pathname) == 0) {
      return idx;
    }
  }
  return 2;
}

int fs_read(int fd, void* buf, size_t len) {

  return 0;
}

int fs_write(int fd, const void* buf, size_t len) {

  return 0;
}

int fs_lseek(int fd, size_t offset, int whence) {
    
  return 0;
}

int fs_close(int fd) {
  return 0;
}
