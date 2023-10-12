#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tinyriscv.h"

void regdump(const trv_cpu* cpu){
	printf("pc = 0x%.8x\n\n", cpu->pc);

	for(unsigned int i = 0; i < 32; i++){
		printf("r%.2d = 0x%.8x\t", i, cpu->regs[i]);
		if(i % 2) printf("\n");
	}
}

void help(void){
	printf("Usage: tinyricv [OPTION] [VALUE] ...\n\n");
	printf("  --help               Display this information.\n");
	printf("  --bin <file>         Load <file> into memory.\n");
	printf("  --mem_size <size>    Set memory to have <size> bytes.\n");

	exit(0);
}

int main(int argc, char** argv){

	size_t mem_size = 4096;

	for(unsigned int i = 0; i < argc; i++){
		if(!strcmp(argv[i], "--help")) help();
			
			else if(!strcmp(argv[i], "--mem_size")){
			if(!argv[i + 1]){
				printf("")		
			}
		} mem_size = atoi(argv[i + 1]);
	}

	if(argc < 2){
		//printf("USAGE: tinyricv <file_path>\n");
		exit(0);
	}

	FILE* file = fopen(argv[1], "rb");
	if(!file){
		perror("ERROR: Can't open file: ");
		exit(-1);
	}

	trv_cpu cpu;
	cpu.mem = malloc(4096);

	fseek(file, 0, SEEK_END);
	unsigned int file_size = ftell(file);
	rewind(file);
	fread(cpu.mem, 1, file_size, file);
	fclose(file);

	trv_init(&cpu);

	while(cpu.pc < file_size + trv_MEM_OFFSET) trv_step(&cpu);

	regdump(&cpu);
	free(cpu.mem);
}