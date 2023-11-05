#include "check.h"

int main() {
	int l,r;
	for (l=0; l<N; l++) {
		for (r=l+1; r<=N; r++) {
			reset();
			memcpy(data +l, src+l, r-l);
			check_seq(0, l, 1);
			check_rseq(l, r, N-l);
			check_seq(r, N, r+1);
		}
	}
}
