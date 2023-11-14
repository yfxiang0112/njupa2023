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

  fseek(fp, 0, SEEK_END);
  int size = ftell(fp);
  printf("size=%d\n", size);
  char buf[size*4];

  fseek(fp, 0, SEEK_SET);
  printf("test at :24\n");
  fread(buf, sizeof(char), size, fp);
  buf[size] = '\0';
  printf("test at :26\n");


  SDL_Surface* res = STBIMG_LoadFromMemory(buf, size);
  assert(res);

  printf("test at :31\n");

  printf("w=%d, h=%d\n", res->w, res->h);


  fclose(fp);

  printf("test at :38\n");
  //free(buf);
  printf("test at :40\n");
  return res;

  //return NULL;

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
