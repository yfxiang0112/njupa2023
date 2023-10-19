#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t currkey = inl(KBD_ADDR);
  //kbd->keydown = (currkey!=AM_KEY_NONE);
  kbd->keycode = currkey;
  printf("%d, %d", currkey, currkey==AM_KEY_NONE);
}
