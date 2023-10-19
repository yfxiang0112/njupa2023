#include <cpu/ftrace.h>

Funct funct_tab[512];
int func_num = 0;

/* struct to record call stack (usint linked lst) */
int call_cnt = 0;
typedef struct call_node {
	int func_ind;
	struct call_node* next;
} call_node;
call_node *call_stack;

void init_ftrace(const char* elf_file) {

	/* load elf file */
	FILE *fp;
	int succ;
	fp = fopen(elf_file, "r");
	if (fp == NULL) { panic("elf file not found"); }

	/* read elf header infomation from elf file */
	Elf32_Ehdr header;
	succ = fread(&header, sizeof(Elf32_Ehdr), 1, fp);
	if (!succ) { panic("fail to read head"); }
	if (header.e_ident[0]!=0x7f||header.e_ident[1]!='E'||header.e_ident[2]!='L'||header.e_ident[3]!='F') { panic("not an elf file. "); }

	/* malloc space for sections, find offset and read sections */
	Elf32_Shdr *sections = (Elf32_Shdr*)malloc(sizeof(Elf32_Shdr)*header.e_shnum);
	succ = fseek(fp, header.e_shoff, SEEK_SET);
	if (succ){ panic("fail to find sections"); }
	succ = fread(sections, sizeof(Elf32_Shdr)*header.e_shnum, 1, fp);
	if (!succ){ panic("fail to read sections"); }

	/* search symtab from sections */
	int st_idx = 0;
	int sst_idx;
	for (int i=0; i<header.e_shnum; i++) {
		if (sections[i].sh_type == SHT_SYMTAB) { st_idx = i; }
	}
	if (st_idx==0) { panic("Symbo table not found"); }
	sst_idx = sections[st_idx].sh_link;

	/* read entries of symbol table from elf file */
	int st_num = sections[st_idx].sh_size/sections[st_idx].sh_entsize;
	Elf32_Sym symtab[st_num];
	for (int i=0; i<st_num; i++) {
		rewind(fp);
		succ = fseek(fp, sections[st_idx].sh_offset + i*sizeof(Elf32_Sym), SEEK_SET);
		if (succ){ panic("fail to find sections"); }
		succ = fread(&symtab[i], sizeof(Elf32_Sym), 1, fp);
		if (!succ){ panic("fail to read sections"); }
	}

	/* read function name string from string table */
	char* name_str = (char*)malloc(sections[sst_idx].sh_size);
	rewind(fp);
	succ = fseek(fp, sections[sst_idx].sh_offset, SEEK_SET);
	if (succ){ panic("fail to find sections"); }
	succ = fread(name_str, sections[sst_idx].sh_size, 1, fp);
	if (!succ){ panic("fail to read sections"); }

	/* filter function symbols from symtab */
	for (int i=0; i<st_num; i++) {
		if (ELF32_ST_TYPE(symtab[i].st_info) == STT_FUNC) {
			func_num ++;
		}
	}

	int idx = 0;
	for (int i=0; i<st_num; i++) {
		if (ELF32_ST_TYPE(symtab[i].st_info) == STT_FUNC) {
			funct_tab[idx].addr = symtab[i].st_value;
			funct_tab[idx].size = symtab[i].st_size;
			strncpy(funct_tab[idx].name, name_str+symtab[i].st_name, 64);
			idx ++;
		}
	}

	call_stack = (call_node*)malloc(sizeof(call_node));
}

void rec_ftrace(vaddr_t addr, vaddr_t pc, uint32_t inst_val) {
#ifdef CONFIG_FTRACE
	if (!CONFIG_FTRACE) { return; }

	/* case: return */
	if (inst_val == 0x00008067) {
		for (int i=0; i<func_num; i++) {
			if (pc >= funct_tab[i].addr && pc <= funct_tab[i].addr+funct_tab[i].size) {

				/* using stack, pop function returns that are not recorded */
				call_node *st_top_call = call_stack;
        call_node *temp;
				while (st_top_call->func_ind != i) {
					call_cnt--;
					printf("0x%x%*s%s [%s]\n", pc, call_cnt, " ", ANSI_FMT("ret ", ANSI_FG_YELLOW), funct_tab[st_top_call->func_ind].name);
          temp = st_top_call;
					st_top_call = st_top_call->next;
          free(temp);
				}
				call_stack = st_top_call -> next;
        //free(st_top_call);
				
				/* current return */
				call_cnt --;
				printf("0x%x%*s%s [%s]\n", pc, call_cnt, " ", ANSI_FMT("ret ", ANSI_FG_YELLOW), funct_tab[i].name);
				return;
			}
		}
	}

	/* case: call */
	for (int i=0; i<func_num; i++) {
		if (addr == funct_tab[i].addr) {
			printf("0x%x%*s%s [%s @0x%x]\n", pc, call_cnt, " ", ANSI_FMT("call", ANSI_FG_CYAN), funct_tab[i].name, addr);
			call_cnt ++;

			/* use stack, record all function calls */
			call_node *new_call = (call_node*)malloc(sizeof(call_node));
			new_call->func_ind = i;
			new_call->next = call_stack;
			call_stack = new_call;

			return;
		}
	}

#endif
}

