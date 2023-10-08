#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

static inline void store8(uint8_t* mem, const uint32_t addr, const uint8_t val){
	mem[addr - MEM_OFFSET] = val;
}

static inline void store16(uint8_t* mem, const uint32_t addr, 
	const uint16_t val){

	*(uint16_t*)(mem + addr - MEM_OFFSET) = val;
}

static inline void store32(uint8_t* mem, const uint32_t addr, 
	const uint32_t val){

	*(uint32_t*)(mem + addr - MEM_OFFSET) = val;
}

void jump(uint32_t* pc, const uint32_t addr){
	if(addr % 4 != 0){
		fprintf(stderr, "EXCEPTION: instruction-address-misaligned\n");
		exit(1);
	}

	*pc = addr;
}

void B_type(uint32_t* pc, const uint8_t func3, const uint32_t regs[32], 
	const uint8_t rs1, const uint8_t rs2, const int16_t imm){

	*pc -= 4;

	switch(func3){
	case /*BEQ*/ 0x0: 
		if(regs[rs1] == regs[rs2]) jump(pc, *pc + (int32_t)imm); 
		break;

	case /*BNE*/ 0x1: 
		if(regs[rs1] != regs[rs2]) jump(pc, *pc + (int32_t)imm); 
		break;

	case /*BLT*/ 0x4: 
		if((int32_t)regs[rs1] < (int32_t)regs[rs2]){
			jump(pc, *pc + (int32_t)imm); 
		}
		break;

	case /*BGE*/ 0x5:
		if((int32_t)regs[rs1] >= (int32_t)regs[rs2]){
			jump(pc, *pc + (int32_t)imm);
		}
		break;

	case /*BLTU*/ 0x6: 
		if(regs[rs1] < regs[rs2]) jump(pc, *pc + (int32_t)imm); 
		break;

	case /*BGEU*/ 0x7: if(regs[rs1] >= regs[rs2]) 
		jump(pc, *pc + (int32_t)imm); 
		break;

	default: *pc += 4; break;
	}
}

void L_type(const uint8_t* mem, const uint8_t func3, uint32_t regs[32], 
	const uint8_t rd, const uint8_t rs1, const int16_t imm){

	const uint32_t addr = regs[rs1] + (int32_t)imm;

	switch(func3){
	case /*LB*/ 0x0: regs[rd] = (int32_t)(int8_t)load8(mem, addr); break;
	case /*LH*/ 0x1: regs[rd] = (int32_t)(int16_t)load16(mem, addr); break;
	case /*LW*/ 0x2: regs[rd] = load32(mem, addr); break;
	case /*LBU*/ 0x4: regs[rd] = load8(mem, addr); break;
	case /*LHU*/ 0x5: regs[rd] = load16(mem, addr); break;
	}
}

void S_type(uint8_t* mem, const uint8_t func3, const uint32_t regs[32], 
	const uint8_t rs1, const uint8_t rs2, const int16_t imm){

	const uint32_t addr = regs[rs1] + (int32_t)imm;

	switch(func3){
	case /*SB*/ 0x0: store8(mem, addr, regs[rs2]); break;
	case /*SH*/ 0x1: store16(mem, addr, regs[rs2]); break;
	case /*SW*/ 0x2: store32(mem, addr, regs[rs2]); break;
	}
}

void I_type(const uint8_t func3, const uint8_t func7, uint32_t regs[32], 
	const uint8_t rd, const uint8_t rs1, const uint8_t rs2, const int16_t imm){

	switch(func3){
	case /*ADDI*/ 0x0: regs[rd] = regs[rs1] + (int32_t)imm; break;
	case /*SLTI*/ 0x2: regs[rd] = (int32_t)regs[rs1] < (int32_t)imm; break;
	case /*SLTIU*/ 0x3: regs[rd] = regs[rs1] < (uint32_t)(int32_t)imm; break;
	case /*XORI*/ 0x4: regs[rd] = regs[rs1] ^ (int32_t)imm; break;
	case /*ORI*/ 0x6: regs[rd] = regs[rs1] | (int32_t)imm; break;
	case /*ANDI*/ 0x7: regs[rd] = regs[rs1] & (int32_t)imm; break;
	case /*SLLI*/ 0x1: regs[rd] = regs[rs1] << rs2; break;
	case /*SRLI/SRAI*/ 0x5:
		if(func7 == /*SRLI*/ 0x00) regs[rd] = regs[rs1] >> rs2;
		else if(func7 == /*SRAI*/ 0x20) regs[rd] = (int32_t)regs[rs1] >> rs2;
		break;
	} 
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
	#define imm_S  (imm_I & 0xffffffe0 | rd)

	#define imm_B  (((int32_t)(inst & 0x80000000) >> 19) | \
		((inst & 0x80) << 4) | (inst >> 20 & 0x7e0) | (inst >> 7 & 0x1e))

	#define imm_J  (((int32_t)(inst & 0x80000000) >> 11) | (inst & 0xff000) | \
        (inst >> 9 & 0x800) | (inst >> 20 & 0x7fe))

	//execute
	cpu->regs[0] = 0;

	switch(opcode){
	case /*LUI*/ 0x37: cpu->regs[rd] = inst & 0xfffff000; break;
	case /*AUIPC*/ 0x17: cpu->regs[rd] = cpu->pc + (inst & 0xfffff000); break;
	case /*JAL*/ 0x6f: 
		cpu->regs[rd] = cpu->pc; jump(&cpu->pc, cpu->pc + imm_J - 4); 
		break;

	case /*JALR*/ 0x67: 
		const uint32_t pc_copy = cpu->pc;
		jump(&cpu->pc, (cpu->regs[rs1] + (int32_t)imm_I) & 0xfffffffe);
		cpu->regs[rd] = pc_copy;
		break;

	case 0x63: B_type(&cpu->pc, func3, cpu->regs, rs1, rs2, imm_B); break;
	case 0x03: L_type(cpu->mem, func3, cpu->regs, rd, rs1, imm_I); break;
	case 0x23: S_type(cpu->mem, func3, cpu->regs, rs1, rs2, imm_S); break;
	case 0x13: I_type(func3, func7, cpu->regs, rd, rs1, rs2, imm_I); break;
	case 0x33: R_type(func3, func7, cpu->regs, rd, rs1, rs2); break;
	}
}

int main(){

}