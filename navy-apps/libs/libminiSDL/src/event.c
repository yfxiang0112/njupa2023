#include <string.h>
#include <NDL.h>
#include <SDL.h>

#define keyname(k) #k,
#define keynum(k) #k

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};


int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  char ndl_buf[64];
  int succ;
  succ = NDL_PollEvent(ndl_buf, sizeof(ndl_buf));
  if (succ != 0) {
    char keystr[64], keydown;
    sscanf(ndl_buf, "k%c %s", &keydown, keystr);

    ev->type = keydown=='d' ? SDL_KEYDOWN : SDL_KEYUP;

    for (int i=0; i<NUM_KEYS; i++) {
      if (strcmp(keystr, keyname[i]) == 0) {
        ev->key.keysym.sym = i;
        keystat[i] = keydown=='d' ? 1 : 0;
        break;
      }
    }
  }
  else { ev->key.keysym.sym = 0; }
  return succ;
}

int SDL_WaitEvent(SDL_Event *event) {
  while(1) {
    char ndl_buf[64];
    if (NDL_PollEvent(ndl_buf, sizeof(ndl_buf))) {
      char keystr[64], keydown;

      sscanf(ndl_buf, "k%c %s", &keydown, keystr);

      event->type = keydown=='d' ? SDL_KEYDOWN : SDL_KEYUP;

      for (int i=0; i<NUM_KEYS; i++) {
        if (strcmp(keystr, keyname[i]) == 0) {
          event->key.keysym.sym = i;
          keystat[i] = keydown=='d' ? 1 : 0;
          break;
        }
      }

      printf("key event %d %d %s\n", event->type, event->key.keysym.sym, keyname[event->key.keysym.sym]);

      return 1;
    }
  }

  return 0;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {

  char ndl_buf[64];
  int succ;
  succ = NDL_PollEvent(ndl_buf, sizeof(ndl_buf));
  if (succ != 0) {
    char keystr[64], keydown;
    sscanf(ndl_buf, "k%c %s", &keydown, keystr);
    for (int i=0; i<NUM_KEYS; i++) {
      if (strcmp(keystr, keyname[i]) == 0) {
        keystat[i] = keydown=='d' ? 1 : 0;
        break;
      }
    }
  }

  /*
  int num = numkeys ? *numkeys : NUM_KEYS;
  for (int i=0; i<num; i++) {
  }
  */
  return keystat;
}
