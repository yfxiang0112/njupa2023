#include <cpu/ftrace.h>

Funct funct_tab[128];

void init_ftrace(const char* elf_file) {

	/* load elf file */
	FILE *fp;
	int succ;
	fp = fopen(elf_file, "r");
	printf("elf=%s\n", elf_file);
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
	int cnt = 0;
	for (int i=0; i<st_num; i++) {
		if (ELF32_ST_TYPE(symtab[i].st_info) == STT_FUNC) {
			cnt ++;
		}
	}
	//Funct temp[cnt];
	//*funct_tab = temp;

	int idx = 0;
	for (int i=0; i<st_num; i++) {
		if (ELF32_ST_TYPE(symtab[i].st_info) == STT_FUNC) {
			funct_tab[idx].addr = symtab[i].st_value;
			funct_tab[idx].size = symtab[i].st_size;
			strncpy(funct_tab[idx].name, name_str+symtab[i].st_name, 64);
			idx ++;
		}
	}
	
}

void rec_ftrace() {
	for (int i=0; i<128; i++) {
		printf("funct%d: %s, %x, %d\n", i, funct_tab[i].name, funct_tab[i].addr, funct_tab[i].size);
	}
}
