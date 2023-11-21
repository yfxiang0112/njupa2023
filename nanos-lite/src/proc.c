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
  //context_kload(&pcb[0], hello_fun, (void*)0);
  context_uload(&pcb[0], "/bin/hello", 
                ((char* const[]){"--skip", "test1", "test2", "test3", NULL} ),
                ((char* const[]) {NULL} ));
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
  for (; argv[n_arg]!=NULL; n_arg++) { arg_len += strlen(argv[n_arg]) +2; }
  for (; envp[n_env]!=NULL; n_env++) { env_len += strlen(argv[n_env]) +2; }
  uintptr_t arg_ptr[n_arg], env_ptr[n_env];


  usp -= sizeof(uintptr_t); *((uintptr_t*)usp) = 0;
  if (*envp) {
    for (int i=n_env-1; i>=0; i--) {
      usp -= strlen(envp[i])+1;
      memcpy((char*)usp, envp[i], strlen(envp[i])+1);
      env_ptr[i] = usp;
    }
  }
  if (*argv) {
    for (int i=n_arg-1; i>=0; i--) {
      usp -= strlen(argv[i])+1;
      memcpy((char*)usp, argv[i], strlen(argv[i])+1);
      arg_ptr[i] = usp;
    }
  }


  if (*envp) {
    usp -= sizeof(uintptr_t); *((uintptr_t*)usp) = 0;
    usp -= sizeof(env_ptr);
    memcpy((char*)usp, env_ptr, sizeof(env_ptr));
  }
  if (*argv) {
    usp -= sizeof(uintptr_t); *((uintptr_t*)usp) = 0;
    usp -= sizeof(arg_ptr);
    memcpy((char*)usp, arg_ptr, sizeof(arg_ptr));
  }


  usp -= sizeof(uintptr_t);
  *((uintptr_t*)usp) = n_arg;
  (n_pcb->cp)->GPRx = usp;
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
