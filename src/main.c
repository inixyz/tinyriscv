#include <stdint.h>

typedef struct{
	uint32_t registers[32], pc;
	uint8_t* memory;
}tinyrv_cpu;

const uint32_t MEMORY_OFFSET = 0x80000000;

static inline uint32_t load32(const uint8_t* memory, const uint32_t addr){
	return *(uint32_t*)(memory + addr - MEMORY_OFFSET);
}

static inline uint32_t select_bits(const uint32_t val, const uint8_t end, 
	const uint8_t start){

	const uint8_t len = end - start + 1;
	return val >> start & (1 << len - 1);
}

void tinyrv_step(tinyrv_cpu* cpu){
	//instruction fetch
	const uint32_t inst = load32(cpu->memory, cpu->pc);
	cpu->pc += 4;

	//instruction decoding
	#define rd select_bits(inst, 11, 7)
	#define rs1 select_bits(inst, 19, 15)
	#define rs2 select_bits(inst, 24, 20)

	const uint8_t opcode = select_bits(inst, 6, 0);
}

int main(){
	tinyrv_cpu cpu;
	cpu.pc = MEMORY_OFFSET;
}
