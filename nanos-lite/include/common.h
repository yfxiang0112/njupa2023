#ifndef __COMMON_H__
#define __COMMON_H__

/* Uncomment these macros to enable corresponding functionality. */
#define HAS_CTE
#define HAS_VME
//#define MULTIPROGRAM
//#define TIME_SHARING
//#define ENABLE_STRACE
//

#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <debug.h>

#define IMAGE_FILE_1 "/bin/nterm"
#define IMAGE_FILE_2 "/bin/nterm"
#define IMAGE_FILE_3 "/bin/menu"


size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();

int fs_open(const char *pathname, int flags, int mode);
int fs_read(int fd, void* buf, size_t len);
int fs_write(int fd, const void* buf, size_t len);
int fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);

void do_syscall(Context *c);

#endif
