#include <unistd.h>
#include <stdio.h>

int test0 = 0;
int test1 = 1;

int main() {
  write(1, "Hello World!\n", 13);
  printf("test0=%d, test1=%d\n", test0, test1);
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
