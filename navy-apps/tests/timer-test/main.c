#include <stdio.h>
#include <assert.h>
#include <sys/time.h>
#include <NDL.h>

extern int _gettimeofday(struct timeval *tv, struct timezone *tz);

int main() {

  struct timeval tv = {0};
  struct timezone tz = {0};
  NDL_Init(0);

  int sec = NDL_GetTicks() / 1000000;
  
  while (1) {
    while(NDL_GetTicks() /1000000 < sec);

    printf("%d\n", sec);
    sec ++;
  }

  return 0;
}
