#include <common.h>
#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_EVENT, FD_FBINFO, FD_FBDEV};

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
  [FD_STDIN]  = {"stdin",          0, 0, 0, invalid_read,  invalid_write},
  [FD_STDOUT] = {"stdout",         0, 0, 0, invalid_read,  serial_write},
  [FD_STDERR] = {"stderr",         0, 0, 0, invalid_read,  serial_write},
  [FD_EVENT]  = {"/dev/events",    0, 0, 0, events_read,   invalid_write},
  [FD_FBINFO] = {"/proc/dispinfo", 0, 0, 0, dispinfo_read, invalid_write},
  [FD_FBDEV]  = {"/dev/fb",        0, 0, 0, invalid_read,  fb_write},
#include "files.h"
};

#define NR_FILES sizeof(file_table)/sizeof(Finfo)

void init_fs() {
  // TODO: initialize the size of /dev/fb
  file_table[FD_FBDEV].size = io_read(AM_GPU_CONFIG).vmemsz;
}


int fs_open(const char *pathname, int flags, int mode) {
  for (int idx=0; idx<NR_FILES; idx++) {
    if (strcmp(file_table[idx].name , pathname) == 0) {
      file_table[idx].open_offset = 0;
      if (!file_table[idx].read && !file_table[idx].write) {
        file_table[idx].read = ramdisk_read;
        file_table[idx].write = ramdisk_write;
      }
      return idx;
    }
  }

  printf("file not found: %s\n", pathname);
  //assert(0);
  return 2;
}

int fs_read(int fd, void* buf, size_t len) {
  assert(fd<NR_FILES);

  switch(fd) {
    case FD_STDIN:
      return 0;
    case FD_STDOUT: case FD_STDERR: case FD_FBDEV:
      return file_table[fd].read(0,0,0);
    case FD_EVENT: case FD_FBINFO:
      return file_table[fd].read(buf, 0, len);

    default:
      size_t off;
      size_t f_off=file_table[fd].open_offset, f_size=file_table[fd].size, f_addr=file_table[fd].disk_offset;
      if (f_off + len > f_size) { len = f_size - f_off; }

      off = f_addr + f_off;
      file_table[fd].read(buf, off, len);
      file_table[fd].open_offset += len;
      return len;
  }
}

int fs_write(int fd, const void* buf, size_t len) {
  assert(fd<NR_FILES);

  switch(fd) {
    case FD_STDIN: case FD_EVENT: case FD_FBINFO:
      return file_table[fd].write(0,0,0);
    case FD_STDOUT: case FD_STDERR:
      return file_table[fd].write(buf, 0, len);
      
    case FD_FBDEV:
    default:
      size_t off;
      size_t f_off=file_table[fd].open_offset, f_size=file_table[fd].size, f_addr=file_table[fd].disk_offset;
      if (f_off + len > f_size) { len = f_size - f_off; }
    
      off = f_addr + f_off;
      file_table[fd].open_offset += len;
      return file_table[fd].write(buf, off, len);
  }
}

int fs_lseek(int fd, size_t offset, int whence) {
  assert(fd>2 && fd<NR_FILES);

  if (whence == SEEK_SET) {
    if (offset > file_table[fd].size || offset < 0) { panic("file operation exceed max size"); }
    file_table[fd].open_offset = offset;
  }
  else if (whence == SEEK_CUR) {
    size_t cur = file_table[fd].open_offset;
    if (offset+cur > file_table[fd].size || offset+cur < 0) { panic("file operation exceed max size"); }
    file_table[fd].open_offset += offset;
  }
  else if (whence == SEEK_END) {
    file_table[fd].open_offset = file_table[fd].size + offset;
  }
    
  return file_table[fd].open_offset;
}

int fs_close(int fd) {
  if (fd<=2) {return 0;}
  assert(fd>2 && fd<NR_FILES);

  file_table[fd].open_offset = 0;
  //TODO:
  //file_table[fd].read = invalid_read;
  //file_table[fd].write = invalid_write;
  return 0;
}
