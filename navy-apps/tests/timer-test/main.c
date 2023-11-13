#include <stdio.h>
#include <assert.h>

int main() {

  struct timeval *tv;
  struct timezone *tz;
  
  while (1) {

    _syscall_(SYS_gettimeofday, tv, tz, 0);
    printf("%ld\n", tv->tv_usec);
  }

  return 0;
}
