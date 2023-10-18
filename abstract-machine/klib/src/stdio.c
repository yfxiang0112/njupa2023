#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

char singlech[2];
char fmt_buf[65];

char* itoa(int num, char* buf, unsigned int base) {
	size_t arr[64] = {0};
	size_t i = 63;

  unsigned int val;

	if (num==0) {
		*buf = 0;
		*(buf+1) = '\0';
		return buf;
	}

	if (num<0) {
		*buf = '-';
		buf ++;
		val = -num;
	} else { val = num; }

	while (val>0 && i>=0) {
		arr[i] = val % base;
		val /= base;
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

char* ftoa(double num, char* buf) {
	size_t arr[64] = {0};
	size_t i = 0, len = 0;

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

  int dec = (int) num;
  num = num - (double) dec;

  itoa(dec, buf, 10);

  buf += strlen(buf);
  *buf = '.';
  buf ++;

	while (num>0 && i<64) {
    num *= 10;
		arr[i] = (int)num;
		num -= arr[i];
		i++;
	}
  len = i;

	for (i=0; i<len; i++) {
		*buf = arr[i] + '0';
		buf ++;
	}
	*buf = '\0';

	return buf;
}

char* parse_fmt(const char** fmt, va_list ap, int *cnt) {

  int d;
  //double f;

	if (**fmt == '%') {
		*fmt = *fmt+1;

		switch (**fmt) {
      case '%':
        break;

			case 'd':
				d = va_arg(ap, int);
        *cnt = *cnt+1;
				itoa(d, fmt_buf, 10);
        return fmt_buf;

        /*
			case 'f':
				f = va_arg(ap, double);
        *cnt = *cnt+1;
				ftoa(f, fmt_buf);
        return fmt_buf;
        */

			case 's':
        *cnt = *cnt+1;
				return va_arg(ap, char*);

			default:
				break;
		}

	}

  singlech[0] = **fmt;
  singlech[1] = '\0';
  return singlech;

}

/////////////////////////////////////////

int printf(const char *fmt, ...) {
	va_list ap;
	char *s;

	int cnt = 0;

	va_start(ap, fmt);
	while (*fmt != '\0') {

    s = parse_fmt(&fmt, ap, &cnt);
    putstr(s);

		fmt ++;
	}
	
	va_end(ap);

	return cnt;
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

    s = parse_fmt(&fmt, ap, &cnt);
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
