#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
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
  uint32_t screen_w = inl(VGACTL_ADDR) >> 16; 
  
  for (int y=0; y<ctl->h; y++) {
    for (int x=0; x<ctl->w; x++) {
      uint32_t off_p = ((y * ctl->w) + x);
      uint32_t off_s = (((y+ctl->y) * screen_w) + (x+ctl->x)) * sizeof(uint32_t);
      outl(FB_ADDR+off_s, ((uint32_t*)(ctl->pixels))[off_p]);

    }
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }

}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
