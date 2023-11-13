#include <stdio.h>
#include <assert.h>
#include <sys/time.h>
#include <NDL.h>

extern int _gettimeofday(struct timeval *tv, struct timezone *tz);

int main() {

  struct timeval tv = {0};
  struct timezone tz = {0};
  NDL_Init(0);

  printf("size=%d\n", sizeof(tv.tv_usec));

  int sec = NDL_GetTicks() / 1000000;
  
  while (1) {

    while(NDL_GetTicks() /1000000 < sec);

    //_syscall_(19, (uintptr_t)tv, (uintptr_t)tz, 0);
    //_gettimeofday(&tv, &tz);
    //uint32_t usec = tv.tv_usec;
    //uint32_t usec = NDL_GetTicks();
    //if (usec % 1000000 == 0) {
      printf("%d\n", sec);
      //printf("tick\n");
    //}

    sec ++;
  }

  return 0;
}
