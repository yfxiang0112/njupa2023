typedef struct Funct {
	vaddr_t addr;
	word_t size;
	char name[65];
} Funct;


void recr_ftrace();
