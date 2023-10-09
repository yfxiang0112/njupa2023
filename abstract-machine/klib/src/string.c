#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
	size_t i = 0;
	while (s[i] != '\0') {
		i ++;
	}
	return i;
}

char *strcpy(char *dst, const char *src) {
	size_t i = 0;
	for (i=0; dst[i] != '\0' && src[i] != '\0'; i++) {
		dst[i] = src[i];
	}
	dst[i] = '\0';
	return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
	size_t i=0;
	
	for (i=0; i<n & src[i]!='\0', i++) {
		dest[i] = src[i];
	}
	for (; i<n; i++a) {
		dest[i] = '\0';
	}
	return dest;
}

char *strcat(char *dst, const char *src) {
	size_t i=0;
	size_t len = strlen(dst);

	for (j=0; src[j]!='\0'; j++) {
		dst[len+j] = src[j];
	}
	dst[len+j] = '\0';
	return dst;
}

int strcmp(const char *s1, const char *s2) {
  panic("Not implemented");
}

int strncmp(const char *s1, const char *s2, size_t n) {
  panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
  panic("Not implemented");
}

void *memmove(void *dst, const void *src, size_t n) {
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
  panic("Not implemented");
}

int memcmp(const void *s1, const void *s2, size_t n) {
  panic("Not implemented");
}

#endif
