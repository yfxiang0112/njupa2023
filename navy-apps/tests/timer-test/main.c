#include <stdio.h>
#include <assert.h>
#include <sys/time.h>

extern int _gettimeofday(struct timeval *tv, struct timezone *tz);

int main() {

  struct timeval tv = {0};
  struct timezone tz = {0};

  printf("size=%d\n", sizeof(tv.tv_usec));
  
  while (1) {

    //_syscall_(19, (uintptr_t)tv, (uintptr_t)tz, 0);
    _gettimeofday(&tv, &tz);
    uint32_t usec = tv.tv_usec;
    if (usec % 500 == 0) {
      printf("%d\n", usec);
    }
  }

  return 0;
}
