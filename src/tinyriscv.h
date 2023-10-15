#ifndef TINYRISCV_H
#define TINYRISCV_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;

typedef struct{
	u32 regs[32], pc, mem_size;
	u8* mem;
}trv_cpu;

const u32 trv_MEM_OFFSET = 0x80000000;

static inline u8 load8(const u8* mem, const u32 addr){
	return mem[addr - trv_MEM_OFFSET];
}

static inline u16 load16(const u8* mem, const u32 addr){
	return *(u16*)(mem + addr - trv_MEM_OFFSET);
}

static inline u32 load32(const u8* mem, const u32 addr){
	return *(u32*)(mem + addr - trv_MEM_OFFSET);
}

static inline void store8(u8* mem, const u32 addr, const u8 val){
	mem[addr - trv_MEM_OFFSET] = val;
}

static inline void store16(u8* mem, const u32 addr, const u16 val){
	*(u16*)(mem + addr - trv_MEM_OFFSET) = val;
}

static inline void store32(u8* mem, const u32 addr, const u32 val){
	*(u32*)(mem + addr - trv_MEM_OFFSET) = val;
}

void trv_B_type(u32* pc, const u8 func3, const u32 regs[32], const u8 rs1, 
	const u8 rs2, const i16 imm){

	switch(func3){
	case /*BEQ*/ 0x0: if(regs[rs1] == regs[rs2]) *pc += (i32)imm - 4; break;
	case /*BNE*/ 0x1: if(regs[rs1] != regs[rs2]) *pc += (i32)imm - 4; break;
	case /*BLT*/ 0x4: 
		if((i32)regs[rs1] < (i32)regs[rs2]) *pc += (i32)imm - 4;
		break;

	case /*BGE*/ 0x5: 
		if((i32)regs[rs1] >= (i32)regs[rs2]) *pc += (i32)imm - 4; 
		break;

	case /*BLTU*/ 0x6: if(regs[rs1] < regs[rs2]) *pc += (i32)imm - 4; break;
	case /*BGEU*/ 0x7: if(regs[rs1] >= regs[rs2]) *pc += (i32)imm - 4; break;
	}
}

void trv_L_type(const u8* mem, const u8 func3, u32 regs[32], const u8 rd, 
	const u8 rs1, const i16 imm){

	const u32 addr = regs[rs1] + (i32)imm;

	switch(func3){
	case /*LB*/ 0x0: regs[rd] = (i32)(i8)load8(mem, addr); break;
	case /*LH*/ 0x1: regs[rd] = (i32)(i16)load16(mem, addr); break;
	case /*LW*/ 0x2: regs[rd] = load32(mem, addr); break;
	case /*LBU*/ 0x4: regs[rd] = load8(mem, addr); break;
	case /*LHU*/ 0x5: regs[rd] = load16(mem, addr); break;
	}
}

void trv_S_type(u8* mem, const u8 func3, const u32 regs[32], const u8 rs1, 
	const u8 rs2, const i16 imm){

	const u32 addr = regs[rs1] + (i32)imm;

	switch(func3){
	case /*SB*/ 0x0: store8(mem, addr, regs[rs2]); break;
	case /*SH*/ 0x1: store16(mem, addr, regs[rs2]); break;
	case /*SW*/ 0x2: store32(mem, addr, regs[rs2]); break;
	}
}

void trv_I_type(const u8 func3, const u8 func7, u32 regs[32], const u8 rd, 
	const u8 rs1, const u8 rs2, const i16 imm){

	switch(func3){
	case /*ADDI*/ 0x0: regs[rd] = regs[rs1] + (i32)imm; break;
	case /*SLTI*/ 0x2: regs[rd] = (i32)regs[rs1] < (i32)imm; break;
	case /*SLTIU*/ 0x3: regs[rd] = regs[rs1] < (u32)(i32)imm; break;
	case /*XORI*/ 0x4: regs[rd] = regs[rs1] ^ (i32)imm; break;
	case /*ORI*/ 0x6: regs[rd] = regs[rs1] | (i32)imm; break;
	case /*ANDI*/ 0x7: regs[rd] = regs[rs1] & (i32)imm; break;
	case /*SLLI*/ 0x1: regs[rd] = regs[rs1] << rs2; break;
	case /*SRLI/SRAI*/ 0x5:
		if(func7 == /*SRLI*/ 0x00) regs[rd] = regs[rs1] >> rs2;
		else if(func7 == /*SRAI*/ 0x20) regs[rd] = (i32)regs[rs1] >> rs2;
		break;
	} 
}

void tinyriscv_R_type(const u8 func3, const u8 func7, u32 x[32], const u8 rd,
	const u8 rs1, const u8 rs2){

	#define ADD  (x[rd] = x[rs1] + x[rs2])
	#define SUB  (x[rd] = x[rs1] - x[rs2])
	#define SLL  (x[rd] = x[rs1] << (x[rs2] & 0x1f))
	#define SLT  (x[rd] = (i32)x[rs1] < (i32)x[rs2])
	#define SLTU (x[rd] = x[rs1] < x[rs2])
	#define XOR  (x[rd] = x[rs1] ^ x[rs2])
	#define SRL  (x[rd] = x[rs1] >> (x[rs2] & 0x1f))
	#define SRA  (x[rd] = (i32)x[rs1] >> (x[rs2] & 0x1f))
	#define OR   (x[rd] = x[rs1] | x[rs2])
	#define AND  (x[rd] = x[rs1] & x[rs2])

	switch(func3){
	case 0: func7 ? SUB : ADD; break; 
	case 1: SLL; break; case 2: SLT; break; 
	case 3: SLTU; break; case 4: XOR; break;
	case 5: func7 ? SRA : SRL; break;
	case 6: OR; break; case 7: AND; break;
	}
}

void trv_step(trv_cpu* cpu){
	//fetch
	const u32 inst = load32(cpu->mem, cpu->pc);
	cpu->pc += 4;

	//decode
	#define opcode (inst & 0x7f)
	#define func3 (inst >> 12 & 0x7)
	#define func7 (inst >> 25 & 0x7f)
	#define rd (inst >> 7 & 0x1f)
	#define rs1 (inst >> 15 & 0x1f)
	#define rs2 (inst >> 20 & 0x1f)
	#define imm_I ((i32)inst >> 20)
	#define imm_S (imm_I & 0xffffffe0 | rd)

	#define imm_B (((i32)(inst & 0x80000000) >> 19) | \
		((inst & 0x80) << 4) | (inst >> 20 & 0x7e0) | (inst >> 7 & 0x1e))

	#define imm_J (((i32)(inst & 0x80000000) >> 11) | (inst & 0xff000) | \
        (inst >> 9 & 0x800) | (inst >> 20 & 0x7fe))

	//execute
	cpu->regs[0] = 0;

	switch(opcode){
	case /*LUI*/ 0x37: cpu->regs[rd] = inst & 0xfffff000; break;
	case /*AUIPC*/ 0x17: cpu->regs[rd] = cpu->pc + (inst & 0xfffff000); break;
	case /*JAL*/ 0x6f: cpu->regs[rd] = cpu->pc; cpu->pc += imm_J - 4; break;
	case /*JALR*/ 0x67: 
		const u32 last_pc = cpu->pc;
		cpu->pc = (cpu->regs[rs1] + (i32)imm_I) & 0xfffffffe;
		cpu->regs[rd] = last_pc;
		break;

	case 0x63: trv_B_type(&cpu->pc, func3, cpu->regs, rs1, rs2, imm_B); break;
	case 0x03: trv_L_type(cpu->mem, func3, cpu->regs, rd, rs1, imm_I); break;
	case 0x23: trv_S_type(cpu->mem, func3, cpu->regs, rs1, rs2, imm_S); break;
	case 0x13: trv_I_type(func3, func7, cpu->regs, rd, rs1, rs2, imm_I); break;
	case 0x33: trv_R_type(func3, func7, cpu->regs, rd, rs1, rs2); break;
	}
}

void trv_init(trv_cpu* cpu){
	cpu->regs[2] = trv_MEM_OFFSET + cpu->mem_size;
	cpu->pc = trv_MEM_OFFSET;
}

#endif //TINYRISCV_H