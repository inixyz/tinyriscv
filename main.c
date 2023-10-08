#include <stdint.h>

typedef struct{
	uint32_t regs[32], pc;
	uint8_t* mem;
}Cpu;

const uint32_t MEM_OFFSET = 0x80000000;

static inline uint8_t load8(const uint8_t* memory, const uint32_t addr){
	return memory[addr - MEM_OFFSET];
}

static inline uint16_t load16(const uint8_t* memory, const uint32_t addr){
	return *(uint16_t*)(memory + addr - MEM_OFFSET);
}

static inline uint32_t load32(const uint8_t* memory, const uint32_t addr){
	return *(uint32_t*)(memory + addr - MEM_OFFSET);
}

static inline uint8_t decode_opcode(const uint32_t inst){
	return inst & 0x7f;
}

static inline uint8_t decode_func3(const uint32_t inst){
	return inst >> 12 & 0x7;
}

static inline uint8_t decode_func7(const uint32_t inst){
	return inst >> 25 & 0x7f;
}

static inline uint8_t decode_rd(const uint32_t inst){
	return inst >> 7 & 0x1f;
}

static inline uint8_t decode_rs1(const uint32_t inst){
	return inst >> 15 & 0x1f;
}

static inline uint8_t decode_rs2(const uint32_t inst){
	return inst >> 20 & 0x1f;
}

// static inline int16_t decode_imm_I(const uint32_t inst){
	
// }

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

// void L_type(){

// 	switch(func3){
// 	case /*LB*/ 0x0: regs[rd] = (int32_t)(int8_t)load8(mem, regs[rs1] + ) break;
// 	}
// }

void step(Cpu* cpu){
	//instruction fetch
	const uint32_t inst = load32(cpu->mem, cpu->pc);
	cpu->pc += 4;

	//instruction decoding
	#define opcode decode_opcode(inst)
	#define func3 decode_func3(inst)
	#define func7 decode_func7(inst)
	#define rd decode_rd(inst)
	#define rs1 decode_rs1(inst)
	#define rs2 decode_rs2(inst)

	//execute
	switch(opcode){
	case /*R-type*/ 0x33: R_type(func3, func7, cpu->regs, rd, rs1, rs2); break;
	}
}

int main(){

}