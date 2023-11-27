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
  context_uload(&pcb[1], IMAGE_FILE, 
                ((char* const[]) {"--skip", NULL} ),
                //((char* const[]) { NULL} ),
                ((char* const[]) { NULL} ));
  context_uload(&pcb[2], "/bin/hello", 
                ((char* const[]) { NULL} ),
                ((char* const[]) { NULL} ));
  switch_boot_pcb();

  Log("Initializing processes...");

  //naive_uload(NULL, IMAGE_FILE);

  // load program here

}

void context_kload(PCB* n_pcb, void (*entry)(void *), void *arg) {
  n_pcb->cp = kcontext((Area) { n_pcb->stack, n_pcb + 1 }, entry, arg);
}

size_t context_uload(PCB* n_pcb, const char* filename, char *const argv[], char *const envp[]) {
  //fb_write(NULL, 0, io_read(AM_GPU_CONFIG).width*io_read(AM_GPU_CONFIG).width);
  protect(&(n_pcb->as));
  io_write(AM_GPU_FBDRAW, 0, 0, NULL, io_read(AM_GPU_CONFIG).width, io_read(AM_GPU_CONFIG).height, true);
  void *new_stack = new_page(STACK_SIZE / PGSIZE);
  uintptr_t usp = (uintptr_t)(new_stack + STACK_SIZE - 1);

  uintptr_t ustack_va = (uintptr_t)(n_pcb->as.area.end) - STACK_SIZE;
  uintptr_t usp_va = (uintptr_t)(ustack_va + STACK_SIZE - 1);
  for (int i=0; i<STACK_SIZE/PGSIZE; i++) {
    //printf("usatck_va=%x, ustack_pa=%x\n", ustack_va+ PGSIZE*i, new_stack+ PGSIZE*i);
    map(&(n_pcb->as), (char*)(ustack_va+ PGSIZE*i), (char*)(new_stack+ PGSIZE*i), 0b111);
  }
  
  int n_arg=0, n_env=0;
  for (; argv[n_arg]!=NULL; n_arg++); 
  for (; envp[n_env]!=NULL; n_env++);
  n_arg ++;
  uintptr_t arg_ptr[n_arg], env_ptr[n_env];


  usp -= sizeof(uintptr_t); *((uintptr_t*)usp) = 0;
  usp_va -= sizeof(uintptr_t);
  if (*envp) {
    for (int i=n_env-1; i>=0; i--) {
      usp -= strlen(envp[i])+1;
      usp_va -= strlen(envp[i])+1;
      memcpy((char*)usp, envp[i], strlen(envp[i])+1);
      env_ptr[i] = usp_va;
    }
  }
  if (*argv) {
    for (int i=n_arg-2; i>=0; i--) {
      usp -= strlen(argv[i])+1;
      usp_va -= strlen(argv[i])+1;
      memcpy((char*)usp, argv[i], strlen(argv[i])+1);
      arg_ptr[i+1] = usp_va;
    }
  }
  usp -= strlen(filename)+1;
  usp_va -= strlen(filename)+1;
  memcpy((char*)usp, filename, strlen(filename)+1);
  arg_ptr[0] = usp_va;


  usp -= sizeof(uintptr_t); *((uintptr_t*)usp) = 0;
  usp_va -= sizeof(uintptr_t);
  if (n_env >= 0) {
    usp -= sizeof(env_ptr);
    usp_va -= sizeof(env_ptr);
    memcpy((char*)usp, env_ptr, sizeof(env_ptr));
  }
  usp -= sizeof(uintptr_t); *((uintptr_t*)usp) = 0;
  usp_va -= sizeof(uintptr_t);
  if (n_arg >= 0) {
    usp -= sizeof(arg_ptr);
    usp_va -= sizeof(arg_ptr);
    memcpy((char*)usp, arg_ptr, sizeof(arg_ptr));
  }



  uintptr_t entry = loader(n_pcb, filename);
  if (!entry || entry==-2) {
    return -2;
  }

  n_pcb->cp = ucontext(&(n_pcb->as), (Area) { (void*)&(n_pcb->stack[0]), (void*)(n_pcb + 1) }, (void*)entry, usp_va);

  usp -= sizeof(uintptr_t);
  usp_va -= sizeof(uintptr_t);
  *((uintptr_t*)usp) = n_arg;
  (n_pcb->cp)->GPRx = usp_va;
  
  /*
  for (uintptr_t i=usp; i<(uintptr_t)(new_stack + 8*PGSIZE - 1); i++) {
    printf("%x %02x %c\n", i, *((char*)i), *((char*)i));
  }
  */

  return 0;
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
