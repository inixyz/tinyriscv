#include <stdio.h>
#include "tinyriscv.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>

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

static inline void set_fg_color(const Color color){
	printf("\x1b[%dm", color);
}

void regdump(const tinyriscv_hart* hart){
	printf("\n");
	printf(" ABI   Reg Hex      │ ABI   Reg Hex     \n");
	printf("────────────────────┼───────────────────\n");

	const char abi[][6] = {
		"zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
		"s0/fp", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
		"a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
		"s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
	};

	for(unsigned int i = 0; i < 32; i++){
		printf(" %-5s", abi[i]);
		
		set_fg_color(BRIGHT_YELLOW);
		printf(i < 10 ? " x%d " : " x%d", i);
		set_fg_color(RESET);

		if(hart->x[i]) set_fg_color(BRIGHT_GREEN);
		printf(" %-8x", hart->x[i]);
		set_fg_color(RESET);

		printf(i % 2 ? " \n" : " │");
	}

	printf(" pc   ");
	set_fg_color(BRIGHT_YELLOW);
	printf(" x32");
	if(hart->pc) set_fg_color(BRIGHT_GREEN);
	printf(" %-8x", hart->pc);
	set_fg_color(RESET);

	printf(" │\n\n");
}

void help(const char* message){
	if(strlen(message)) puts(message);

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
	if(argc < 2) help("Not enough arguments.");

	for(unsigned int i = 1; i < argc; i++){
		if(!strcmp(argv[i], "--help")) help(NULL);
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
			printf("Invalid option: %s\n", argv[i]);			
			printf("Try 'tinyriscv --help' for more information.\n");
		}
	}

	tinyriscv_hart hart;
	hart.mem = malloc(hart.mem_size = mem_size);

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
	fread(hart.mem, 1, file_size, file);
	fclose(file);

	//program execution
	tinyriscv_init(&hart);
	while(hart.pc < file_size + tinyriscv_MEM_OFFSET) tinyriscv_step(&hart);

	if(do_regdump) regdump(&hart);

	//cleanup
	free(hart.mem);
}