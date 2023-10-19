#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
  /*
  int i;
  int w = io_read(AM_GPU_CONFIG).width;
  int h = io_read(AM_GPU_CONFIG).height;
  uint32_t *fb = (uint32_t*)(uintptr_t)FB_ADDR;
  for (i=0; i<w*h; i++) fb[i]=i;
  outl(SYNC_ADDR, 1);
  */
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t gpuconfig = inl(VGACTL_ADDR);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = gpuconfig>>16, .height = gpuconfig&0xffff,
    .vmemsz = (gpuconfig>>16) * (gpuconfig&0xffff) * sizeof(uint32_t)
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  uint32_t *fb = (uint32_t*)(uintptr_t)FB_ADDR;
  for (int x=ctl->x; x<ctl->w; x++) {
    for (int y=ctl->y; y<ctl->h; y++) {
      uint32_t offset = ((x*ctl->w)+y) * sizeof(uint32_t);
//      outl(FB_ADDR+offset, ((uint32_t*)ctl->pixels)[offset]);
      fb[offset] = ((uint32_t*)ctl->pixels)[offset];

    }
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }

}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
