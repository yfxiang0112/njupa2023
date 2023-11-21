#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
static int curr_idx=3;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}

void init_proc() {
  context_kload(&pcb[0], hello_fun, (void*)0);
  context_uload(&pcb[1], "/bin/hello", ((char* const[]){"--skip", "--test", NULL} ), ((char* const[]) {NULL} ));
  switch_boot_pcb();

  Log("Initializing processes...");

  //naive_uload(NULL, IMAGE_FILE);

  // load program here

}

void context_kload(PCB* n_pcb, void (*entry)(void *), void *arg) {
  n_pcb->cp = kcontext((Area) { n_pcb->stack, n_pcb + 1 }, entry, arg);
}

void context_uload(PCB* n_pcb, const char* filename, char *const argv[], char *const envp[]) {
  //fb_write(NULL, 0, io_read(AM_GPU_CONFIG).width*io_read(AM_GPU_CONFIG).width);
  uintptr_t entry = loader(n_pcb, filename);

  if (!entry) {
    return;
  }


  n_pcb->cp = ucontext(NULL, (Area) { (void*)&(n_pcb->stack[0]), (void*)(n_pcb + 1) }, (void*)entry);
  uintptr_t usp = (uintptr_t)(heap.end);

  int n_arg=0, n_env=0;
  int arg_len=0, env_len=0;
  for (; argv[n_arg]!=NULL; n_arg++) { arg_len += strlen(argv[n_arg]) +1; }
  for (; envp[n_env]!=NULL; n_env++) { env_len += strlen(argv[n_env]) +1; }

  printf("argc=%d, envp=%d\n", n_arg, n_env);
  printf("arglen=%d, envp len=%d\n", arg_len, env_len);

  printf("envp=%x\n", (uintptr_t)envp);
  printf("argv=%x\n", (uintptr_t)argv);
  printf("len=%d\n", sizeof(*argv));

  usp -= sizeof(uintptr_t); *((uintptr_t*)usp) = 0;

  /*
  usp -= env_len;
  char *stack_envp = (char*)usp;
  memcpy(stack_envp, *envp, env_len);
  */

  usp -= arg_len;
  char *stack_argv = (char*)usp;
  memcpy(stack_argv, *argv, arg_len);

  /*
  usp -= sizeof(uintptr_t); *((uintptr_t*)usp) = 0;
  for (int i=n_env-1; i>=0; i--) {
    *((uintptr_t*)usp) = (uintptr_t)(&(stack_envp[i]));
    usp -= sizeof(uintptr_t);
  }
*/
  usp -= sizeof(uintptr_t); *((uintptr_t*)usp) = 0;
  for (int i=n_arg-1; i>=0; i--) {
    *((uintptr_t*)usp) = (uintptr_t)(&(stack_argv[i]));
    usp -= sizeof(uintptr_t);
  }

  *((size_t*)usp) = n_arg;
  (n_pcb->cp)->GPRx = usp;

  for (uintptr_t i=usp; i<(uintptr_t)(heap.end); i++) {
    printf(" %02x", *((char*)i));
  }
  printf("\n");
  for (uintptr_t i=usp; i<(uintptr_t)(heap.end); i++) {
    printf("%3c", *((char*)i));
  }
  printf("\n");
}

Context* schedule(Context *prev) {
  current->cp = prev;
  while(1) {
    curr_idx = (curr_idx+1)%4;
    if (pcb[curr_idx].cp != 0) {
      current = &pcb[curr_idx];
      break;
    }
  }
  return current->cp;
}
