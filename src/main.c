#include <stdint.h>

typedef struct{
	uint32_t registers[32], pc;
	uint8_t* memory;
}tinyrv_cpu;

uint32_t tinyrv_ram_load(const tinyrv_cpu* cpu, const uint32_t addr, 
	const uint8_t size){

	uint32_t val = 0;

	for(uint8_t i = 0; i < size; i++){
		val |= (uint32_t)cpu->memory[addr + i] << (i * 8);
	}

	return val;
}

#include <stdio.h>

int main(){
	uint8_t memory[100];
	tinyrv_cpu cpu;
	cpu.memory = memory;

	memory[0] = 0xAA;
	memory[1] = 0xBB;
	memory[2] = 0xCC;
	memory[3] = 0xDD;

	printf("%x\n", tinyrv_ram_load(&cpu, 0, 1));
	printf("%x\n", tinyrv_ram_load(&cpu, 0, 2));
	printf("%x\n", tinyrv_ram_load(&cpu, 0, 3));
	printf("%x\n", tinyrv_ram_load(&cpu, 0, 4));
}
