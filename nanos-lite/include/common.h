#ifndef __COMMON_H__
#define __COMMON_H__

/* Uncomment these macros to enable corresponding functionality. */
#define HAS_CTE
//#define HAS_VME
//#define MULTIPROGRAM
//#define TIME_SHARING
#define ENABLE_STRACE
//

#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <debug.h>

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();

int fs_open(const char*pathname, int flags, int mode);

void do_syscall(Context *c);

#endif
