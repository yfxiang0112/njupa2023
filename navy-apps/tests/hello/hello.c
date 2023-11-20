#include <unistd.h>
#include <assert.h>
#include <stdio.h>

int main() {
  write(1, "Hello World!\n", 13);

  /*
  uint32_t temp;
  asm volatile ("mv %0, sp" : "=r"(temp));
  printf("sp=%x\n", temp);
  */

  int i = 2;
  volatile int j = 0;
  while (1) {
    j ++;
    if (j == 10000) {
      printf("Hello World from Navy-apps for the %dth time!\n", i ++);
      j = 0;
    }
  }
  return 0;
}
