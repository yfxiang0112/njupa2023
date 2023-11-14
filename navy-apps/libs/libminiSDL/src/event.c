#include <string.h>
#include <NDL.h>
#include <SDL.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  while(1) {
    char ndl_buf[64];
    if (NDL_PollEvent(ndl_buf, sizeof(ndl_buf))) {
      char keystr[64], keydown;

      sscanf(ndl_buf, "k%c %s", &keydown, keystr);

      event->type = keydown=='d' ? SDL_KEYDOWN : SDL_KEYUP;

      for (int i=0; i<=SDLK_PAGEDOWN) {
        if (strcmp(keystr, keyname[i]) == 0) {
          event->key.keysym.sym = i;
          break;
        }
      }

      return 1;
    }
  }

  return 0;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
