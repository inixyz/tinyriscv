#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "tinyriscv.h"

typedef enum{
	RESET   =  0, DEFAULT        = 39,
	BLACK   = 30, BRIGHT_BLACK   = 90,
	RED     = 31, BRIGHT_RED     = 91,
	GREEN   = 32, BRIGHT_GREEN   = 92,
	YELLOW  = 33, BRIGHT_YELLOW  = 93,
	BLUE    = 34, BRIGHT_BLUE    = 94,
	MAGENTA = 35, BRIGHT_MAGENTA = 95,
	CYAN    = 36, BRIGHT_CYAN    = 96,
	WHITE   = 37, BRIGHT_WHITE   = 97	
}Color;

static inline void set_color(const Color color){
	printf("\033[%dm", color);
}

void regdump(const trv_cpu* cpu){
	printf("\n ABI   Reg Hex      │ ABI   Reg Hex\n");
	printf("────────────────────┼───────────────────\n");

	for(unsigned int i = 0; i < 32; i++){
		printf(" ");
		switch(i){
		case  0: printf("zero "); break; case  1: printf("ra   "); break;
		case  2: printf("sp   "); break; case  3: printf("gp   "); break;
		case  4: printf("tp   "); break; case  5: printf("t0   "); break;
		case  6: printf("t1   "); break; case  7: printf("t2   "); break;
		case  8: printf("s0/fp"); break; case  9: printf("s1   "); break;
		case 10: printf("a0   "); break; case 11: printf("a1   "); break;
		case 12: printf("a2   "); break; case 13: printf("a3   "); break;
		case 14: printf("a4   "); break; case 15: printf("a5   "); break;
		case 16: printf("a6   "); break; case 17: printf("a7   "); break;
		case 18: printf("s2   "); break; case 19: printf("s3   "); break;
		case 20: printf("s4   "); break; case 21: printf("s5   "); break;
		case 22: printf("s6   "); break; case 23: printf("s7   "); break;
		case 24: printf("s8   "); break; case 25: printf("s9   "); break;
		case 26: printf("s10  "); break; case 27: printf("s11  "); break;
		case 28: printf("t3   "); break; case 29: printf("t4   "); break;
		case 30: printf("t5   "); break; case 31: printf("t5   "); break;
		}

		printf(" ");
		set_color(BRIGHT_YELLOW);
		printf(i < 10 ? "x%d " : "x%d", i);
		set_color(RESET);

		printf(" ");
		if(cpu->regs[i]) set_color(BRIGHT_GREEN);
		printf("%-8x", cpu->regs[i]);
		set_color(RESET);

		printf(i % 2 ? " \n" : " │");
	}

	printf(" pc    ");
	set_color(BRIGHT_YELLOW);
	printf("x32");
	if(cpu->pc) set_color(BRIGHT_GREEN);
	printf(" %-8x", cpu->pc);
	set_color(RESET);

	printf(" │\n\n");
}

void help(void){
	printf("Usage: tinyricv [OPTION] [VALUE] ...\n\n");
	printf("  --help               Display this information.\n");
	printf("  --bin <file>         Load <file> into memory.\n");
	printf("  --mem_size <size>    Set memory to have <size> bytes.\n");
	printf("  --regdump            Dump registers at execution halt.\n");

	exit(0);
}

int main(int argc, char** argv){
	char file_path[256] = "";
	size_t mem_size = 4096;
	unsigned int do_regdump = 0;

 	//command line arguments
	if(argc < 2){
		printf("Not enough arguments.\n");
		help();
	}
	for(unsigned int i = 1; i < argc; i++){
		if(!strcmp(argv[i], "--help")) help();
		else if(!strcmp(argv[i], "--bin")){
			assert("Invalid file path." && argv[i + 1]);
			strncpy(file_path, argv[i++ + 1], 256);
		}
		else if(!strcmp(argv[i], "--mem_size")){
			assert("Invalid [mem_size] value." && argv[i + 1]);
			mem_size = atoi(argv[i++ + 1]);
		}
		else if(!strcmp(argv[i], "--regdump")) do_regdump = 1;
		else{
			printf("Invalid argument: %s\n", argv[i]);			
			help();
		}
	}

	trv_cpu cpu;
	cpu.mem = malloc(cpu.mem_size = mem_size);

	//file handling
	FILE* file = fopen(file_path, "rb");
	if(!file){
		perror("ERROR: Can't open file");
		exit(-1);
	}

	//load program into memory
	fseek(file, 0, SEEK_END);
	const unsigned int file_size = ftell(file);
	rewind(file);
	fread(cpu.mem, 1, file_size, file);
	fclose(file);

	//program execution
	trv_init(&cpu);
	while(cpu.pc < file_size + trv_MEM_OFFSET) trv_step(&cpu);

	//cleanup
	if(do_regdump) regdump(&cpu);
	free(cpu.mem);
}