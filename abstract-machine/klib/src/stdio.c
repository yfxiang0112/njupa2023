#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

char singlech[2];
char fmt_buf[129];

char* itoa(uint64_t num, char* buf, uint32_t base, uint32_t len) {
	uint32_t arr[64] = {0};
	uint32_t i = 63;

	if (num==0) {
		*buf = '0';
		*(buf+1) = '\0';
		return buf;
	}

  //TODO: when input num = 2147483647, output=-1
	if (base==10 && ((num>0x7fffffff && len==32) || (num>0x7fffffffffffffff && len==64))) {
		*buf = '-';
		buf ++;
    if (len == 32) {
      num = (uint32_t)0 - (uint32_t)num;
    } else {
		  num = 0-num;
    }
	}

	while (num>0 && i>=0) {
		arr[i] = num % base;
		num /= base;
		i--;
	}

	i = 0;
	while (arr[i] == 0) {
		i++;
	}
	for (; i<64; i++) {
		if(arr[i]<10) {*buf = arr[i] + '0'; }
    else {*buf = arr[i] + 'a' - 10; }
		buf ++;
	}
	*buf = '\0';

	return buf;
}


char* parse_fmt(const char** fmt, va_list *ap, int *cnt) {

  uint32_t d;
  uint64_t ld;
  uint32_t wid=0;

  char arg_buf[65] = {'\0'};

	if (**fmt == '%') {
		*fmt = *fmt+1;

    if (**fmt == '0') {
      for(int i=0; i<129; i++) { fmt_buf[i] = '0'; }
      *fmt = *fmt+1;
    } else {
      for(int i=0; i<129; i++) { fmt_buf[i] = ' '; }
    }

    while (**fmt >= '0' && **fmt <= '9') {
      wid *= 10;
      wid += (**fmt - '0');
      *fmt = *fmt + 1;
    }

		switch (**fmt) {
      case '%':
        break;

			case 'd':
				d = va_arg(*ap, uint32_t);
        *cnt = *cnt+1;
				itoa(d, arg_buf, 10, 32);
        break;

			case 'x':
				d = va_arg(*ap, uint32_t);
        *cnt = *cnt+1;
				itoa(d, arg_buf, 16, 32);
        break;

			case 'l':
		    *fmt = *fmt+1;
        if (**fmt == 'd') { 
			    ld = va_arg(*ap, uint64_t);
          *cnt = *cnt+1;
			    itoa(ld, arg_buf, 10, 64);
          break;
        }
        break;

			case 's':
        *cnt = *cnt+1;
        strcpy(arg_buf, va_arg(*ap, char*));
        break;

			default:
				break;
		}

    if (wid > strlen(arg_buf)) { wid -= strlen(arg_buf); }
    else { wid = 0; }
    strcpy(fmt_buf+wid, arg_buf);
    return fmt_buf;

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

    s = parse_fmt(&fmt, &ap, &cnt);
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

    s = parse_fmt(&fmt, &ap, &cnt);
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
