#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  asm volatile ("ebreak");

  int argc = *args;
  char **argv = (char **)(args + 1);
  char **envp = (char **)(args + argc + 2);

  char *empty[] =  {NULL };
  environ = envp;
  exit(main(argc, argv, envp));
  assert(0);
}
