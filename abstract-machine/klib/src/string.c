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
	for (i=0; src[i] != '\0'; i++) {
		dst[i] = src[i];
	}
	
	dst[i] = '\0';
	return dst;
}

//TODO:test
char *strncpy(char *dst, const char *src, size_t n) {
	size_t i=0;
	
	for (i=0; i<n && src[i]!='\0'; i++) {
		dst[i] = src[i];
	}
	for (; i<n; i++) {
		dst[i] = '\0';
	}
	return dst;
}

char *strcat(char *dst, const char *src) {
	size_t i=0;
	size_t len = strlen(dst);

	for (i=0; src[i]!='\0'; i++) {
		dst[len+i] = src[i];
	}
	dst[len+i] = '\0';
	return dst;
}

int strcmp(const char *s1, const char *s2) {
	size_t i=0;
	for (i=0; s1[i]!='\0' && s2[i]!='\0'; i++) {
		if (s1[i] != s2[i]) {
			return s1[i]-s2[i];
		}
	}
	if (s1[i] != '\0') { return s1[i]; }
	else if (s2[i] != '\0') {	return -s2[i];	}
	else {	return 0;	}
}

//TODO:test
int strncmp(const char *s1, const char *s2, size_t n) {
	size_t i=0;
	for (i=0; i<n && s1[i]!='\0' && s2[i]!='\0'; i++) {
		if (s1[i] != s2[i]) {
			return s1[i]-s2[i];
		}
	}
	if (i<n) {
		if (s1[i] != '\0') { return s1[i];	}
		else if (s2[i] != '\0') { return -s2[i];	}
	}
	return 0;
}

void *memset(void *s, int c, size_t n) {
	for (size_t i=0; i<n; i++) {
		*((unsigned char*)s+ i) = c;
	}
	return s;
}

//TODO: test
void *memmove(void *dst, const void *src, size_t n) {
	unsigned char arr[n];
	for (size_t i=0; i<n; i++) {
		arr[i] = *((unsigned char*)src +i);
	}
	for (size_t i=0; i<n; i++) {
		*((unsigned char*)dst +i) = arr[i];
	}
	return dst;
}

//TODO: test
void *memcpy(void *dst, const void *src, size_t n) {
	for (size_t i=0; i<n; i++) {
		*((unsigned char*)dst +i) = *((unsigned char*)src +i);
	}
	return dst;
}

int memcmp(const void *s1, const void *s2, size_t n) {
	for (size_t i=0; i<n; i++) {
		if (*((unsigned char*)s1 +i) != *((unsigned char*)s2 +i)) {
			return *((unsigned char*)s1 +i) - *((unsigned char*)s2 +i);
		}
	}
	return 0;
}

#endif
