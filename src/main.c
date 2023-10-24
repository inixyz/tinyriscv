#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

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

////////////////////////////////////////////////////////////////////////////////

static inline void set_fg_color(const Color color){
	printf("\x1b[%dm", color);
}

////////////////////////////////////////////////////////////////////////////////

static inline void clear_screen(void){
	printf("\x1b[H\x1b[J");
}

////////////////////////////////////////////////////////////////////////////////

char getch(void)
{
	struct termios old_term, new_term;

	tcgetattr(STDIN_FILENO, &old_term);
	new_term = old_term;

	new_term.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &new_term);

	char c = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &old_term);
	return c;
}

////////////////////////////////////////////////////////////////////////////////

void help(const char* message){
	if(message) printf("%s\n\n", message);

	printf("Usage: tinyricv [OPTION] [VALUE] ...\n\n");
	printf("  --help               Display this information.\n");
	printf("  --bin <file>         Load <file> into memory.\n");
	printf("  --mem_size <size>    Set memory to have <size> bytes.\n");
	printf("  --regdump            Dump registers at execution halt.\n");
	printf("  --memdump <addr>     Dump 256B of memory from <addr>.\n");
	printf("  --debug              Enable debug mode individual astep.\n");

	exit(0);
}

////////////////////////////////////////////////////////////////////////////////

void regdump(const tinyriscv_hart* hart){
	printf("\n");
	printf(" ABI   Reg Hex      │ ABI   Reg Hex     \n");
	printf(" ───────────────────┼───────────────────\n");

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

////////////////////////////////////////////////////////////////////////////////

void memdump(const tinyriscv_hart* hart, uint32_t addr){
	addr -= tinyriscv_MEM_OFFSET;
	if(addr + 256 > hart->mem_size) help("memdump: Address out of memory range.");

	printf("\n");
	printf(" Address  Memory                                          ASCII\n ");
	for(unsigned int i = 0; i < 73; i++) printf("─");
	printf("\n");

	for(uint32_t end_addr = addr + 256; addr < end_addr; addr += 16){
		set_fg_color(BRIGHT_YELLOW);
		printf(" %.8x ", addr + tinyriscv_MEM_OFFSET);
		set_fg_color(RESET);

		for(uint32_t i = addr; i < addr + 16; i++){
			set_fg_color(hart->mem[i] ? BRIGHT_WHITE : BRIGHT_BLACK);
			printf("%.2x ", hart->mem[i]);
		}

		for(uint32_t i = addr; i < addr + 16; i++){
			const char c = hart->mem[i];
			const unsigned int visible = c > 31 && c < 255 && c != 127;
			set_fg_color(visible ? BRIGHT_WHITE : BRIGHT_BLACK);
			printf("%c", visible ? c : '.');	
		}

		printf("\n");
	}

	set_fg_color(RESET);
	printf("\n");
}

////////////////////////////////////////////////////////////////////////////////

void load_mem_from_file(tinyriscv_hart* hart, const char* file_path){
	FILE* file = fopen(file_path, "rb");
	if(!file){
		perror("ERROR: Can't open file");
		exit(-1);
	}

	fseek(file, 0, SEEK_END);
	const unsigned int file_size = ftell(file);
	rewind(file);

	if(file_size > hart->mem_size){
		printf("ERROR: [file] is bigger than allocated memory.");
		fclose(file);
		exit(-1);
	}

	fread(hart->mem, 1, file_size, file);
	fclose(file);
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv){
	char file_path[256] = "";
	size_t mem_size = 4096;
	uint32_t dump_addr = 0;
	unsigned int do_regdump = 0, do_memdump = 0, do_debug = 0;

 	//command line arguments
	if(argc < 2) help("Not enough arguments.");

	for(unsigned int i = 1; i < argc; i++){
		if(!strcmp(argv[i], "--help")) help(NULL);
		else if(!strcmp(argv[i], "--bin")){
			if(i + 1 >= argc) help("Invalid file path."); 
			strncpy(file_path, argv[i++ + 1], 256);
		}
		else if(!strcmp(argv[i], "--mem_size")){
			if(i + 1 >= argc) help("Invalid [mem_size] value.");
			mem_size = strtol(argv[i++ + 1], NULL, 0);
		}
		else if(!strcmp(argv[i], "--regdump")) do_regdump = 1;
		else if(!strcmp(argv[i], "--memdump")){
			if(i + 1 >= argc) help("Invalid [dump_addr] value.");
			dump_addr = strtol(argv[i++ + 1], NULL, 0);
			do_memdump = 1;
		}
		else if(!strcmp(argv[i], "--debug")) do_debug = 1;
		else{
			printf("Invalid option: %s\n", argv[i]);			
			printf("Try 'tinyriscv --help' for more information.\n");
			exit(-1);
		}
	}

	tinyriscv_hart hart;
	hart.mem = calloc(hart.mem_size = mem_size, 1);

	//load program into memory
	load_mem_from_file(&hart, file_path);

	//program execution
	tinyriscv_init(&hart);
	if(do_debug){
		while(tinyriscv_valid_step(&hart)){
			clear_screen();

			if(do_regdump) regdump(&hart);
			if(do_memdump) memdump(&hart, dump_addr);

			printf("\nPress enter to step.\n");
			while(getch() != 10);

			tinyriscv_step(&hart);
		}
	}
	else while(tinyriscv_valid_step(&hart)) tinyriscv_step(&hart);

	if(do_regdump) regdump(&hart);
	if(do_memdump) memdump(&hart, dump_addr);

	//cleanup
	free(hart.mem);
}

////////////////////////////////////////////////////////////////////////////////