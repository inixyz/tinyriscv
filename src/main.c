#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "tinyriscv.h"

char lf(char c){
	if(c < 32) return 48;
	if(c > 126) return 48;
	return c;
}

void regdump(const trv_cpu* cpu){
	printf("pc = 0x%.8x\n\n", cpu->pc);

	printf("\t\t\t\t\t\t reg 1Dec2Dec3Dec4Dec 1Hx2Hx3Hx4Hx C_h_a_r reg 1Dec2Dec3Dec4Dec 1Hx2Hx3Hx4Hx C_h_a_r\n");
	for(unsigned int i = 0; i < 32; i++){
		printf("r%.2d = 0x%.8x\t", i, cpu->regs[i]);
		if(i % 2){
			printf(" r%.2d ", i-1);
			printf(" %.3hhu %.3hhu %.3hhu %.3hhu ", *((char*)(cpu->regs + i-1)), *(((char*)(cpu->regs + i-1)) + 1), *((char*)(cpu->regs + i-1) + 2), *((char*)(cpu->regs + i-1) + 3));
			printf(" %.2x %.2x %.2x %.2x ", *((char*)(cpu->regs + i-1)) & 0xff, *((char*)(cpu->regs + i-1) + 1) & 0xff, *((char*)(cpu->regs + i-1) + 2) & 0xff, *((char*)(cpu->regs + i-1) + 3) & 0xff);
			printf("%c %c %c %c ", lf(*((char*)(cpu->regs + i-1))), lf(*((char*)(cpu->regs + i-1) + 1)), lf(*((char*)(cpu->regs + i-1) + 2)), lf(*((char*)(cpu->regs + i-1) + 3)));

			printf("r%.2d ", i);
			printf(" %.3hhu %.3hhu %.3hhu %.3hhu ", *((char*)(cpu->regs + i)), *((char*)(cpu->regs + i) + 1), *((char*)(cpu->regs + i) + 2), *((char*)(cpu->regs + i) + 3));
			printf(" %.2x %.2x %.2x %.2x ", *((char*)(cpu->regs + i)), *((char*)(cpu->regs + i) + 1), *((char*)(cpu->regs + i) + 2), *((char*)(cpu->regs + i) + 3));
			printf("%c %c %c %c ", lf(*((char*)(cpu->regs + i))), lf(*((char*)(cpu->regs + i) + 1)), lf(*((char*)(cpu->regs + i) + 2)), lf(*((char*)(cpu->regs + i) + 3)));
			
			printf("\n");
		}
	}
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
	cpu.regs[2] = 0x5c4a4b4c;
	if(do_regdump) regdump(&cpu);
	free(cpu.mem);
}