#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

char singlech[2];

char* itoa(int num, char* buf, size_t base) {
	size_t arr[64] = {0};
	size_t i = 63;

	if (num==0) {
		*buf = 0;
		*(buf+1) = '\0';
		return buf;
	}

	if (num<0) {
		*buf = '-';
		buf ++;
		num = -num;
	}

	while (num>0 && i>=0) {
		arr[i] = (unsigned int)num % base;
		num /= base;
		i--;
	}

	i = 0;
	while (arr[i] == 0) {
		i++;
	}
	for (; i<64; i++) {
		*buf = arr[i] + '0';
		buf ++;
	}
	*buf = '\0';

	return buf;
}

char* parse_fmt(const char* fmt, va_list ap, int *cnt) {

	char buf[65];
  int d;

	if (*fmt == '%') {
		fmt++;

		switch (*fmt) {
			case 'd':
				d = (int)ap;
        *cnt = *cnt+1;
				return itoa(d, buf, 10);

			case 's':
        *cnt = *cnt+1;
				return va_arg(ap, char*);

			default:
				break;
		}

	}

  singlech[0] = *fmt;
  singlech[1] = '\0';
  return singlech;

}

/////////////////////////////////////////

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
	va_list ap;
	char *s;

	int cnt = 0;

	va_start(ap, fmt);
	while (*fmt != '\0') {


    s = parse_fmt(fmt, ap, &cnt );
    memcpy(out, s, strlen(s));

    out += strlen(s);
		fmt ++;
	}
	
	va_end(ap);
	*out = '\0';

	return cnt;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
