#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  yield();

  for (int i=0; i<len; i++) { putch(*((char*)buf + i)); }
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  yield();

  size_t ret;
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  if (ev.keycode == AM_KEY_NONE) return 0;

  ret = snprintf(buf, len, "k%c %s\n", ev.keydown ? 'd' : 'u', keyname[ev.keycode]);
  return ret;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T gpuconfig = io_read(AM_GPU_CONFIG);
  return snprintf(buf, len, "WIDTH : %d\nHEIGHT : %d\n", gpuconfig.width, gpuconfig.height);
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  // TODO: Optimization need towards yield for each call
  
  yield();

  int w = io_read(AM_GPU_CONFIG).width;
  int h = io_read(AM_GPU_CONFIG).height;
  int x = offset%w, y = offset/w;

  if (!buf) { io_write(AM_GPU_FBDRAW, 0, 0, 0, w, h, true); }
  else if (len == w*h) { io_write(AM_GPU_FBDRAW, 0, 0, (uint32_t*)buf, w, h, true); }
  else if (len <= w-x) { io_write(AM_GPU_FBDRAW, x, y, (uint32_t*)buf, len, 1, true); }

  else {
    for (int i=0; i<len; i++) {
      offset += i;
      x = offset%w;
      y = offset/w;
      io_write(AM_GPU_FBDRAW, x, y, (uint32_t*)buf+i, 1, 1, false);
    }
    io_write(AM_GPU_FBDRAW, 0, 0, 0, 0, 0, true);
  }

  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
