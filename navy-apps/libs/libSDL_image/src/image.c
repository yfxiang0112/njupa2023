#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"

SDL_Surface* IMG_Load_RW(SDL_RWops *src, int freesrc) {
  assert(src->type == RW_TYPE_MEM);
  assert(freesrc == 0);
  return NULL;
}

SDL_Surface* IMG_Load(const char *filename) {
  FILE *fp = fopen(filename, "r+");
  assert(fp);

  long size = ftell(fp);
  char buf[size*2];

  fseek(fp, 0, SEEK_SET);
  printf("test at :23\n");
  fread(buf, size, size, fp);

  printf("test at :28\n");

  for (int i=0; i<size/sizeof(uint32_t); i++) {
    printf("%d  ", ((uint32_t*)buf)[i]);
  }
  printf("\n");



  return NULL;

}

int IMG_isPNG(SDL_RWops *src) {
  return 0;
}

SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src) {
  return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
  return "Navy does not support IMG_GetError()";
}
