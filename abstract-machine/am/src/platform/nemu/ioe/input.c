#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  kbd->keycode = inb(KBD_ADDR);
  printf("0x%x\n", inw(KBD_ADDR));
  kbd->keydown = ((uint32_t)inw(KBD_ADDR) >> 15);
}
