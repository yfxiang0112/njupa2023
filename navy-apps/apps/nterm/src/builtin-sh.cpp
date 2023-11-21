#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>
#include <assert.h>

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}

static void sh_handle_cmd(const char *cmd) {
  char cmd_buf[256];
  if (strlen(cmd)>200) {assert(0);}
  char* arg_lst[32];
  int nr_arg = 0;

  strncpy(cmd_buf, cmd, strlen(cmd)-1);
  cmd_buf[strlen(cmd)-1] = 0;

  for (int i=0; cmd_buf[i]!=0; i++) {
    if (cmd_buf[i] != ' ' && cmd_buf != 0 && (i==0 || cmd_buf[i-1]==0)) {
      arg_lst[nr_arg] = cmd_buf + i;
      nr_arg ++;
    }

    if (cmd_buf[i]==' ') { cmd_buf[i] = 0; }
  }
  arg_lst[nr_arg] = 0;

  /*
  for (int i=0; i<nr_arg; i++) {
    printf("%x, %s\n", (uintptr_t)(&arg_lst[i]), arg_lst[i]);
  }
  */

  //////////////////////////////////////////////////

  if (strcmp( arg_lst[1], "poweroff" ) == 0) {
    if (strcmp( arg_lst[0], "sudo" ) == 0) {
      SDL_Quit();
    }
  }

  execvp(arg_lst[0], &(arg_lst[1]));
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();
  setenv("PATH", "/bin:$PATH", 0);

  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
