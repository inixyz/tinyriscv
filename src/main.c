#include "tinyriscv.h"
#include <stdio.h>

void dump_registers(const tinyriscv_Cpu* cpu){
	printf("pc = 0x%.8x\n", cpu->pc);

	printf("\n");

	for(unsigned int i = 0; i < 32; i++){
		printf("r%.2d = 0x%.8x\t", i, cpu->regs[i]);
		if(i % 2) printf("\n");
	}
}

int main(int argc, char** argv){
	tinyriscv_Cpu cpu;
	tinyriscv_init(&cpu);

	

	dump_registers(&cpu);
}