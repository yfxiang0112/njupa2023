#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  int argc = *args, n_env;
  char **argv = (char **)(args + 1);
  char **envp = (char **)(args + argc + 2);
  for (int i=0; i<argc; i++) {
    printf("%s %x\n", (char*)(argv[i]), (uintptr_t)(argv[i]));
  }

  for (int i=0; envp[i]!=NULL; i++) {
    printf("%s %x\n", (char*)(envp[i]), (uintptr_t)(envp[i]));
  }

  //for (int i=0; i<)

  char *empty[] =  {NULL };
  environ = empty;
  exit(main(0, empty, empty));
  assert(0);
}
