#include <stdio.h>
#include <assert.h>
#include <sys/time.h>

uint32_t foo() {
  uint32_t test[100000];

  for (int i=0; i<100000; i++) {
    test[i] = i;
  }

  printf("test\n");
  for (int i=100000-1; i>=0; i--) {
    printf("[%d]=%d\n", i, test[i]);
    test[i] = 100000-i;
  }

  return test[5000];
}

uint32_t bar(int a) {
  int b=1000;
  uint32_t test[a*b];

  for (int i=0; i<a*b; i++) {
    printf("bar[%d] =%d\n", i, test[i]);
    test[i] = 0xffffffff;
  }

  return a;
}

int main() {
  
  foo();

  for (int i=0; i<10; i++) {
    foo();
    bar(i*1000);
  }

  bar(0xffff);

  return 0;
}
