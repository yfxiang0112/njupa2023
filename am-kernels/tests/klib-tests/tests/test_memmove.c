#include "check.h"

int main() {
	int l,r;
	for (l=0; l<N; l++) {
		for (r=l+1; r<=N; r++) {
			reset();
			memmove(data +l, src+l, r-l);
			check_seq(0, l, 1);
			check_rseq(l, r, N-l);
			check_seq(r, N, r+1);
		}
	}

	for (l=1; l<N; l++) {
		reset();
		memmove(data, data+l, N-l);
		check_seq(0, N-l, l+1);
	}

	for (r=N-1; r>=0; r--) {
		reset();
		memmove(data+r, data, N-r);
		check_seq(r, N, 1);
	}
}
