#include "check.h"

int main() {
	int l;
	for (l=0; l<N; l++) {
		reset();
		strcpy(data+l, src);
		check_seq(0, l, 1);
		check_rseq(l, l+N, N);
		check_eq(l+N, l+N+1, 0);
	}
}
