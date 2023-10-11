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
#include <memory/paddr.h>
#include <elf.h>

void init_rand();
void init_log(const char *log_file);
void init_mem();
void init_difftest(char *ref_so_file, long img_size, int port);
void init_ftrace(const char *elf_file);
void init_device();
void init_sdb();
void init_disasm(const char *triple);

static void welcome() {
  Log("Trace: %s", MUXDEF(CONFIG_TRACE, ANSI_FMT("ON", ANSI_FG_GREEN), ANSI_FMT("OFF", ANSI_FG_RED)));
  IFDEF(CONFIG_TRACE, Log("If trace is enabled, a log file will be generated "
        "to record the trace. This may lead to a large log file. "
        "If it is not necessary, you can disable it in menuconfig"));
  Log("Build time: %s, %s", __TIME__, __DATE__);

	printf(ANSI_FMT(" ===", ANSI_FG_RED)ANSI_FMT("===", ANSI_FG_YELLOW)ANSI_FMT("===", ANSI_FG_GREEN)ANSI_FMT("===", ANSI_FG_BLUE)ANSI_FMT("===", ANSI_FG_CYAN)ANSI_FMT("===\n", ANSI_FG_MAGENTA));
	printf(" coding with "ANSI_FMT("P", ANSI_FG_RED)ANSI_FMT("R", ANSI_FG_YELLOW)ANSI_FMT("I", ANSI_FG_GREEN)ANSI_FMT("D", ANSI_FG_BLUE)ANSI_FMT("E", ANSI_FG_CYAN)ANSI_FMT("!\n", ANSI_FG_MAGENTA));
	printf(ANSI_FMT(" ===", ANSI_FG_RED)ANSI_FMT("===", ANSI_FG_YELLOW)ANSI_FMT("===", ANSI_FG_GREEN)ANSI_FMT("===", ANSI_FG_BLUE)ANSI_FMT("===", ANSI_FG_CYAN)ANSI_FMT("===\n", ANSI_FG_MAGENTA));
	
  printf("Welcome to %s-NEMU!\n", ANSI_FMT(str(__GUEST_ISA__), ANSI_FG_YELLOW ANSI_BG_RED));
  printf("For help, type \"help\"\n");
}

#ifndef CONFIG_TARGET_AM
#include <getopt.h>

void sdb_set_batch_mode();

static char *log_file = NULL;
static char *diff_so_file = NULL;
static char *ftrace_elf_file = NULL;
static char *img_file = NULL;
static int difftest_port = 1234;

static long load_img() {
  if (img_file == NULL) {
    Log("No image is given. Use the default build-in image.");
    return 4096; // built-in image size
  }

  FILE *fp = fopen(img_file, "rb");
  Assert(fp, "Can not open '%s'", img_file);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);

  Log("The image is %s, size = %ld", img_file, size);

  fseek(fp, 0, SEEK_SET);
  int ret = fread(guest_to_host(RESET_VECTOR), size, 1, fp);
  assert(ret == 1);

  fclose(fp);
  return size;
}

static int parse_args(int argc, char *argv[]) {
  const struct option table[] = {
    {"batch"    , no_argument      , NULL, 'b'},
    {"log"      , required_argument, NULL, 'l'},
    {"diff"     , required_argument, NULL, 'd'},
    {"port"     , required_argument, NULL, 'p'},
		{"ftrace"   , required_argument, NULL, 'f'},
    {"help"     , no_argument      , NULL, 'h'},
    {0          , 0                , NULL,  0 },
  };
  int o;
  while ( (o = getopt_long(argc, argv, "-bhl:d:p:f:", table, NULL)) != -1) {
    switch (o) {
      case 'b': sdb_set_batch_mode(); break;
      case 'p': sscanf(optarg, "%d", &difftest_port); break;
      case 'l': log_file = optarg; break;
      case 'd': diff_so_file = optarg; break;
			case 'f': ftrace_elf_file = optarg; printf("ELF FILE: %s\n", ftrace_elf_file); break;
      case 1: img_file = optarg; return 0;
      default:
        printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
        printf("\t-b,--batch              run with batch mode\n");
        printf("\t-l,--log=FILE           output log to FILE\n");
        printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
        printf("\t-p,--port=PORT          run DiffTest with port PORT\n");
        printf("\t-f,--elf=FILE           check f-trace with efl file\n");
        printf("\n");
        exit(0);
    }
  }
  return 0;
}

void init_monitor(int argc, char *argv[]) {
  /* Perform some global initialization. */

  /* Parse arguments. */
  parse_args(argc, argv);

  /* Set random seed. */
  init_rand();

  /* Open the log file. */
  init_log(log_file);

  /* Initialize memory. */
  init_mem();

  /* Initialize devices. */
  IFDEF(CONFIG_DEVICE, init_device());

  /* Perform ISA dependent initialization. */
  init_isa();

  /* Load the image to memory. This will overwrite the built-in image. */
  long img_size = load_img();

  /* Initialize differential testing. */
  init_difftest(diff_so_file, img_size, difftest_port);

  /* Initialize the simple debugger. */
  init_sdb();

	/* Initialize the function call trace file and read elf file. */
	IFDEF(CONFIG_FTRACE, init_ftrace(ftrace_elf_file));

#ifndef CONFIG_ISA_loongarch32r
  IFDEF(CONFIG_ITRACE, init_disasm(
    MUXDEF(CONFIG_ISA_x86,     "i686",
    MUXDEF(CONFIG_ISA_mips32,  "mipsel",
    MUXDEF(CONFIG_ISA_riscv,
      MUXDEF(CONFIG_RV64,      "riscv64",
                               "riscv32"),
                               "bad"))) "-pc-linux-gnu"
  ));
#endif

  /* Display welcome message. */
  welcome();
}
#else // CONFIG_TARGET_AM
static long load_img() {
  extern char bin_start, bin_end;
  size_t size = &bin_end - &bin_start;
  Log("img size = %ld", size);
  memcpy(guest_to_host(RESET_VECTOR), &bin_start, size);
  return size;
}

void am_init_monitor() {
  init_rand();
  init_mem();
  init_isa();
  load_img();
	IFDEF(CONFIG_FTRACE, init_ftrace(ftrace_elf_file));
  IFDEF(CONFIG_DEVICE, init_device());
  welcome();
}
#endif

void init_ftrace(const char* elf_file) {

	FILE *fp;
	int a;
	fp = fopen(elf_file, "r");
	printf("elf=%s\n", elf_file);
	if (fp == NULL) { panic("elf file not found"); }

	Elf32_Ehdr header;
	Elf32_Shdr *sections;
	Elf32_Sym *symtab = NULL;
	a = fread(&header, sizeof(Elf32_Ehdr), 1, fp);
	if (a==0) { panic("fail to read head"); }
	if (header.e_ident[0]!=0x7f||header.e_ident[1]!='E'||header.e_ident[2]!='L'||header.e_ident[3]!='F') { panic("not an elf file. "); }

	sections = (Elf32_Shdr *)(&header + header.e_shoff);

	printf("sections check %d\n", sections==NULL);
	printf("shnum=%d\n", header.e_shnum);
	printf("shentsize=%d\n", header.e_shentsize);
	printf("shoff=%x\n", header.e_shoff);
	printf("phnum=%d\n", header.e_phnum);
	printf("SYMTAB=%d\n", SHT_SYMTAB);

	for (int i=0; i<header.e_shnum; i++) {
		printf("sh type %d = %d\n", i, sections[i].sh_type);
		if (sections[i].sh_type == SHT_SYMTAB) {
			printf("find\n");
			symtab = (Elf32_Sym*)((char*)&header + sections[i].sh_offset);
			break;
		}
	}
	
	assert(symtab != NULL);
}
