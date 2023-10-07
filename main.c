#include <stdint.h>

typedef struct{
	uint32_t regs[32], pc;
	uint8_t* mem;
}Cpu;

const uint32_t MEMORY_OFFSET = 0x80000000;

static inline uint32_t load32(const uint8_t* memory, const uint32_t addr){
	return *(uint32_t*)(memory + addr - MEMORY_OFFSET);
}

static inline uint32_t select_bits(const uint32_t val, const uint8_t end, 
	const uint8_t start){

	const uint8_t len = end - start + 1;
	return val >> start & (1 << len - 1);
}

void R_type(const uint8_t func3, const uint8_t func7, uint32_t regs[32], 
	const uint8_t rd, const uint8_t rs1, const uint8_t rs2){

	switch(func3){
	case /*ADD/SUB*/ 0x0:
		if(func7 == /*ADD*/ 0x00) regs[rd] = regs[rs1] + regs[rs2];
		else if(func7 == /*SUB*/ 0x20) regs[rd] = regs[rs1] - regs[rs2];
		break;

	case /*SLL*/ 0x1: regs[rd] = regs[rs1] << (regs[rs2] & 0x1f); break;
	case /*SLT*/ 0x2: regs[rd] = (int32_t)regs[rs1] < (int32_t)regs[rs2]; break;
	case /*SLTU*/ 0x3: regs[rd] = regs[rs1] < regs[rs2]; break;
	case /*XOR*/ 0x4: regs[rd] = regs[rs1] ^ regs[rs2]; break;

	case /*SRL/SRA*/ 0x5: 
		if(func7 == /*SRL*/ 0x00) regs[rd] = regs[rs1] >> (regs[rs2] & 0x1f);
		else if(func7 == /*SRA*/ 0x20) regs[rd] = (int32_t)regs[rs1] >> (regs[rs2] & 0x1f);
		break;

	case /*OR*/ 0x6: regs[rd] = regs[rs1] | regs[rs2]; break;
	case /*AND*/ 0x7: regs[rd] = regs[rs1] & regs[rs2]; break;
	}
}

void step(Cpu* cpu){
	//instruction fetch
	const uint32_t inst = load32(cpu->mem, cpu->pc);
	cpu->pc += 4;

	//instruction decoding
	#define opcode select_bits(inst, 6, 0)
	#define func3 select_bits(inst, 14, 12)
	#define func7 select_bits(inst, 31, 25)
	#define rd select_bits(inst, 11, 7)
	#define rs1 select_bits(inst, 19, 15)
	#define rs2 select_bits(inst, 24, 20)

	//execute
	switch(opcode){
	case /*R-type*/ 0x33: R_type(func3, func7, cpu->regs, rd, rs1, rs2); break;
	//case /*I-type*/ 0x03: I_type(); break;
	//case /*S-type*/ 0x23: S_type(); break;
	//case /*B-type*/ 0x63: B_type(); break;
	}
}

int main(){

}