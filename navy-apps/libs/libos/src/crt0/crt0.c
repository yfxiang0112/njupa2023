#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  printf("%x\n", *args);
  int argc = *args, n_env;
  char *const *argv = (char * const*)( args[1] );
  for (int i=0; i<argc; i++) {
    printf("%x\n", (uintptr_t)(& argv[i] ));
    printf("%s\n", argv[i]);
  }

  //for (int i=0; i<)

  char *empty[] =  {NULL };
  environ = empty;
  exit(main(0, empty, empty));
  assert(0);
}
