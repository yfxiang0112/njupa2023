/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <cpu/cpu.h>
#include <memory/paddr.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

/* coutinue exec */
static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

/* quit. */
static int cmd_q(char *args) {
	nemu_state.state = NEMU_END; 
  return -1;
}

/* run single instruction in sdb. */
static int cmd_si(char *args){
	unsigned int step;
	//TODO sdb_command: si[nu]
	if(args==NULL){
		step=1;
	}else{
		step = atoi(args);
	}
	//TODO handle undesired arg input, TODO check nemu state.
	cpu_exec(step);
	return 0;
}

/* print watchpoint or regi info. */
static int cmd_info(char *args){
	char subCmd='\0';
	if (args!=NULL && strlen(args)==1){subCmd=args[0];}
	switch (subCmd){
		case 'w':
			//TODO
			display_wp();
			break;
		case 'r':
			// pr
			isa_reg_display();	
			break;
		default:
			break;
	}
	return 0;
}

/* scan emory. */
static int cmd_x(char *args){
	//char *expr = args;
	char *endptr;
	uint32_t addr;
	int32_t len;

	/* extract factor from *args. */
	char *len_str = strtok(NULL, " ");
	char *addr_str = strtok(NULL, " ");

	/* turn into int. */
	len = atoi(len_str);
	if (len<=0) { return 0; }
	addr = strtol(addr_str, &endptr, 16);
	
	/* evaluate & increse pmem address. */
	for (int i=0; i<len; i++){
		if(addr-CONFIG_MBASE > CONFIG_MSIZE) {
			printf("Error: Invalid memory address.\n");
			return 0;
		}
		uint32_t res = paddr_read(addr, 4);
		printf("0x%0*x\n",8,res);
		addr+=4;
	}	
	return 0;
}

/* evaluate expression */
static int cmd_p(char *args){
	bool success = true;
	word_t res = expr(args, &success);
	//printf("res: %d, success: %d\n", res, success);
	if (success) {
		printf("%u\n", res);
	}
	return 0;
}

/* add watchpoint */
static int cmd_w(char *args){
	add_wp(args);
	return 0;
}

/* remove watchpoint */
static int cmd_d(char *args) {
	rm_wp(atoi(args));
	return 0;
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
	{ "si", "Run single instruction", cmd_si },
	{ "info", "Print info of reg or watchpoint", cmd_info },
  { "x", "Scan memory", cmd_x },
  { "p", "Print expression", cmd_p },
  { "w", "Set a watchpoint", cmd_w },
  { "d", "Delete a watchpoint", cmd_d },

  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
