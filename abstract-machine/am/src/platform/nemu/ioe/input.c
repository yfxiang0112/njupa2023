#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint16_t curr_key = inw(KBD_ADDR);
  printf("%d\n", curr_key<<8>>8);
  kbd->keycode = (curr_key <<8) >>8;
  kbd->keydown = curr_key>>15;
}
