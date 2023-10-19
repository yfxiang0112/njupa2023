#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t curr_key = inw(KBD_ADDR);
  kbd->keycode = (curr_key <<8) >>8;
  kbd->keydown = curr_key>>15;
}
