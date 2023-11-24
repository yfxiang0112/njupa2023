#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>

static int evtdev = 3;
static int fbinfo = 4;
static int fbdev = 5;
static int screen_w = 0, screen_h = 0;
static int canvas_w = 0, canvas_h = 0;
static uint32_t *fb;

uint32_t NDL_GetTicks() {
  struct timeval tv = {0};
  struct timezone tz = {0};
  gettimeofday(&tv, &tz);
  return tv.tv_usec / 1000;
}

int NDL_PollEvent(char *buf, int len) {
  int succ = 0;
  succ = read(evtdev, buf, len);
  close(evtdev);
  return (succ != 0);
}

void NDL_OpenCanvas(int *w, int *h) {
  char readbuf[32];
  read(fbinfo, readbuf, strlen(readbuf));
  sscanf(readbuf, "WIDTH : %d\nHEIGHT : %d\n", &screen_w, &screen_h);
  fb = malloc(sizeof(uint32_t[screen_w][screen_h]));
  for (int i=0; i<screen_w*screen_h; i++) {
    fb[i] = 0;
  }

  if (*w>screen_w || *w==0) { *w = screen_w; }
  if (*h>screen_h || *h==0) { *h = screen_h; }
  canvas_w = *w; canvas_h = *h;
  
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  /*
  uint32_t fb[screen_w*screen_h];
  */
  /*
  for (int i=0; i<screen_w*screen_h; i++) {
    fb[i] = 0;
  }
  */
  
  /*
  for (uint32_t j=0; j<h; j++) {
    uint32_t off = (j + y + (screen_h-canvas_h)/2) * screen_w + x + (screen_w-canvas_w)/2;
    lseek(fbdev, off, SEEK_SET);
    write(fbdev, pixels +j*w, w);
  }
  close(fbdev);
  */

  //if (x==0 && w==canvas_w && h==1) {
  if (h==1) {
    uint32_t off = (y + (screen_h-canvas_h)/2) * screen_w + x + (screen_w-canvas_w)/2;
    for (int i=0; i<w; i++) {
      fb[off+i] = pixels[i];
    }
    //printf("x=%d, y=%d, w=%d, h=%d, cw=%d, ch=%d\n", x, y, w, h, canvas_w, canvas_h);
    if (y == canvas_h-1) {
      lseek(fbdev, 0, SEEK_SET);
      write(fbdev, fb, screen_w*screen_h);
      close(fbdev);
    }
    return;
  }
  
  for (uint32_t j=0; j<h; j++) {
    uint32_t off = (y + j + (screen_h-canvas_h)/2) * screen_w + x + (screen_w-canvas_w)/2;
    for (int i=0; i<w; i++) {
      fb[off+i] = pixels[j*w+i];
    }
  }
  lseek(fbdev, 0, SEEK_SET);
  write(fbdev, fb, screen_w*screen_h);
  close(fbdev);
  
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  assert(0);

  return 0;
}

void NDL_Quit() {
  exit(0);
}
