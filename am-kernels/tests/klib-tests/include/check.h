#ifndef __TRAP_H__
#define __TRAP_H__

#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#define N 32
uint8_t data[N*2];
uint8_t src[N*2];

__attribute__((noinline))

void reset() {
	int i;
	for (i=0; i<N; i++) {
		data[i] = i+1;
		src[i] = N-i;
	}
	data[N] = 0;
	src[N] = 0;
}

void check_seq(int l, int r, int val) {
	int i;
	for (i=l; i<r; i++) {
		assert(data[i] == val + i - l);
	}
}

void check_rseq(int l, int r, int val) {
	int i;
	for (i=l; i<r; i++) {
		assert(data[i] == val - i + l);
	}
}

void check_eq(int l, int r, int val) {
	int i;
	for (i=l; i<r; i++) {
		assert(data[i] == val);
	}
}

#endif
