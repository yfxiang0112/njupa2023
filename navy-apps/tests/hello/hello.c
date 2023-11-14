#include <unistd.h>
#include <assert.h>
#include <stdio.h>

static int test0 = 0;
const int test1 = 0;
int test2 = 0;

int main() {
  write(1, "Hello World!\n", 13);
  printf("test0=%d, test1=%d, test2=%d\n", test0, test1, test2);
  assert(test2==0);
  int i = 2;
  volatile int j = 0;
  while (1) {
    j ++;
    if (j == 10000) {
      printf("Hello World from Navy-apps for the %dth time!\n", i ++);
      return 0;
      j = 0;
    }
  }
  return 0;
}
