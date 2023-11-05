#include "check.h"

int main() {
  /*
	int l;
	for (l=0; l<N; l++) {
		reset();
		strcpy(data+l, src);
		check_seq(0, l, 1);
		check_rseq(l, l+N, N);
		check_eq(l+N, l+N+1, 0);
	}
  */

  long long int ld = 1;
  printf("%d %ld %d %ld\n", 2147364,  22147483649, 217483648, ld);
  printf("%d %d %d %d\n", -1, -2147483648, 0, -2147483647);
  printf("%d\n", -100);
  printf("%d %d %d\n", -2147483646, -2147483647, 48);
  printf("%d %d %s %d\n", 0, -1, "test", -2147483648);
  printf("%d %d %d %d\n", 1, 2147483646, 0, 2147483647);
  printf("%ld %ld\n", -2147483649, -0x7fffffffffffffff);

  printf("%x %x\n", -2147483648, -0x7fffffff);
  printf("%x %0101x\n", 2147483647, 0x7fffffff);

  printf("%2s %10s\n", "hello", "world");
}
