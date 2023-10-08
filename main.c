#include <stdint.h>

typedef struct{
	uint32_t regs[32], pc;
	uint8_t* mem;
}Cpu;

const uint32_t MEM_OFFSET = 0x80000000;

static inline uint8_t load8(const uint8_t* mem, const uint32_t addr){
	return mem[addr - MEM_OFFSET];
}

static inline uint16_t load16(const uint8_t* mem, const uint32_t addr){
	return *(uint16_t*)(mem + addr - MEM_OFFSET);
}

static inline uint32_t load32(const uint8_t* mem, const uint32_t addr){
	return *(uint32_t*)(mem + addr - MEM_OFFSET);
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
		else if(func7 == /*SRA*/ 0x20){
			regs[rd] = (int32_t)regs[rs1] >> (regs[rs2] & 0x1f);
		}
		break;

	case /*OR*/ 0x6: regs[rd] = regs[rs1] | regs[rs2]; break;
	case /*AND*/ 0x7: regs[rd] = regs[rs1] & regs[rs2]; break;
	}
}

void L_type(const uint8_t func3, uint32_t regs[32], const uint8_t rd, 
	const uint8_t* mem, const uint8_t rs1, const int16_t imm){

	switch(func3){
	case /*LB*/ 0x0: 
		regs[rd] = (int32_t)(int8_t)load8(mem, regs[rs1] + (int32_t)imm); 
		break;

	case /*LH*/ 0x1: 
		regs[rd] = (int32_t)(int16_t)load16(mem, regs[rs1] + (int32_t)imm); 
		break;

	case /*LW*/ 0x2: regs[rd] = load32(mem, regs[rs1] + (int32_t)imm); break;
	case /*LBU*/ 0x4: load8(mem, regs[rs1] + (int32_t)imm); break;
	case /*LHU*/ 0x5: load16(mem, regs[rs1] + (int32_t)imm); break;
	}
}

void step(Cpu* cpu){
	//fetch
	const uint32_t inst = load32(cpu->mem, cpu->pc);
	cpu->pc += 4;

	//decode
	#define opcode (inst & 0x7f)
	#define func3  (inst >> 12 & 0x7)
	#define func7  (inst >> 25 & 0x7f)
	#define rd     (inst >> 7 & 0x1f)
	#define rs1    (inst >> 15 & 0x1f)
	#define rs2    (inst >> 20 & 0x1f)
	#define imm_I  ((int32_t)inst >> 20)

	//execute
	cpu->regs[0] = 0;

	switch(opcode){
	case 0x03: L_type(func3, cpu->regs, rd, cpu->mem, rs1, imm_I); break;
	case 0x33: R_type(func3, func7, cpu->regs, rd, rs1, rs2); break;
	}
}

int main(){

}