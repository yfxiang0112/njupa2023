#include <stdio.h>
#include <assert.h>
#include <sys/time.h>

extern int _gettimeofday(struct timeval *tv, struct timezone *tz);

int main() {

  struct timeval *tv = {0};
  struct timezone *tz = {0};
  
  while (1) {

    //_syscall_(19, (uintptr_t)tv, (uintptr_t)tz, 0);
    _gettimeofday(tv, tz);
    printf("%ld\n", tv->tv_usec);
  }

  return 0;
}
