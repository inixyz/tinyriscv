#ifndef TINYRISCV_H
#define TINYRISCV_H

#include <stdint.h>

typedef uint8_t   u8; typedef int8_t   i8;
typedef uint16_t u16; typedef int16_t i16;
typedef uint32_t u32; typedef int32_t i32;
typedef uint64_t u64; typedef int64_t i64;

typedef struct{
	u32 x[32], pc, mem_size;
	u8* mem;
}tinyriscv_core;

const u32 tinyriscv_MEM_OFFSET = 0x80000000;

#define MEM(size, mem, addr) (*(u##size*)(mem + addr - tinyriscv_MEM_OFFSET))

////////////////////////////////////////////////////////////////////////////////

__attribute__((always_inline)) 
inline void tinyriscv_B_type(const u8 funct3, const u32* x, u32* pc, 
	const u8 rs1, const u8 rs2, const i16 imm){

	switch(funct3){
	case /*BEQ*/  0: if(x[rs1] == x[rs2]) *pc += (i32)imm - 4; break; 
	case /*BNE*/  1: if(x[rs1] != x[rs2]) *pc += (i32)imm - 4; break;
	case /*BLT*/  4: if((i32)x[rs1] < (i32)x[rs2]) *pc += (i32)imm - 4; break; 
	case /*BGE*/  5: if((i32)x[rs1] >= (i32)x[rs2]) *pc += (i32)imm - 4; break;
	case /*BLTU*/ 6: if(x[rs1] < x[rs2]) *pc += (i32)imm - 4; break; 
	case /*BGEU*/ 7: if(x[rs1] >= x[rs2]) *pc += (i32)imm - 4; break;
	}
}

////////////////////////////////////////////////////////////////////////////////

__attribute__((always_inline)) 
inline void tinyriscv_L_type(const u8 funct3, u32* x, const u8* mem, 
	const u8 rd, const u8 rs1, const i16 imm){

	const u32 addr = x[rs1] + (i32)imm;

	switch(funct3){
	case /*LB*/  0: x[rd] = (i32)(i8)MEM(8, mem, addr); break;
	case /*LH*/  1: x[rd] = (i32)(i16)MEM(16, mem, addr); break;
	case /*LW*/  2: x[rd] = MEM(32, mem, addr); break;
	case /*LBU*/ 4: x[rd] = MEM( 8, mem, addr); break;
	case /*LHU*/ 5: x[rd] = MEM(16, mem, addr); break;
	}
}

////////////////////////////////////////////////////////////////////////////////

__attribute__((always_inline)) 
inline void tinyriscv_S_type(const u8 funct3, const u32* x, u8* mem, 
	const u8 rs1, const u8 rs2, const i16 imm){

	const u32 addr = x[rs1] + (i32)imm;

	switch(funct3){
	case /*SB*/ 0: MEM( 8, mem, addr) = x[rs2]; break;
	case /*SH*/ 1: MEM(16, mem, addr) = x[rs2]; break;
	case /*SW*/ 2: MEM(32, mem, addr) = x[rs2]; break;
	}
}

////////////////////////////////////////////////////////////////////////////////

__attribute__((always_inline)) 
inline void tinyriscv_I_type(const u8 funct3, const u8 funct7, u32* x,
	const u8 rd, const u8 rs1, const i16 imm, const u8 shamt){

	switch(funct3){
	case /*ADDI*/  0: x[rd] = x[rs1] + (i32)imm; break;
	case /*SLTI*/  2: x[rd] = (i32)x[rs1] < (i32)imm; break;
	case /*SLTIU*/ 3: x[rd] = x[rs1] < (u32)(i32)imm; break;
	case /*XORI*/  4: x[rd] = x[rs1] ^ (i32)imm; break;
	case /*ORI*/   6: x[rd] = x[rs1] | (i32)imm; break;
	case /*ANDI*/  7: x[rd] = x[rs1] & (i32)imm; break;
	default: 
		switch(funct7 << 3 | funct3){
		case /*SLLI*/ 0x001: x[rd] = x[rs1] << shamt; break;
		case /*SRLI*/ 0x005: x[rd] = x[rs1] >> shamt; break;
		case /*SRAI*/ 0x105: x[rd] = (i32)x[rs1] >> shamt; break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

__attribute__((always_inline)) 
inline void tinyriscv_R_type(const u8 funct3, const u8 funct7, u32* x, 
	const u8 rd, const u8 rs1, const u8 rs2){

	switch(funct7 << 3 | funct3){
	case /*ADD*/  0x000: x[rd] = x[rs1] + x[rs2]; break;
	case /*SUB*/  0x100: x[rd] = x[rs1] - x[rs2]; break;
	case /*SLL*/  0x001: x[rd] = x[rs1] << (x[rs2] & 0x1f); break;
	case /*SLT*/  0x002: x[rd] = (i32)x[rs1] < (i32)x[rs2]; break;
	case /*SLTU*/ 0x003: x[rd] = x[rs1] < x[rs2]; break;
	case /*XOR*/  0x004: x[rd] = x[rs1] ^ x[rs2]; break;
	case /*SRL*/  0x005: x[rd] = x[rs1] >> (x[rs2] & 0x1f); break;
	case /*SRA*/  0x105: x[rd] = (i32)x[rs1] >> (x[rs2] & 0x1f); break;
	case /*OR*/   0x006: x[rd] = x[rs1] | x[rs2]; break;
	case /*AND*/  0x007: x[rd] = x[rs1] & x[rs2]; break;

	#ifdef TINYRISCV_M
	case /*MUL*/    0x008: x[rd] = x[rs1] * x[rs2]; break;
	case /*MULH*/   0x009: 
		x[rd] = ((i64)(i32)x[rs1] * (i64)(i32)x[rs2]) >> 32; 
		break;
	case /*MULHSU*/ 0x00a: 
		x[rd] = ((i64)(i32)x[rs1] * (u64)x[rs2]) >> 32; 
		break;
	case /*MULHU*/  0x00b: x[rd] = ((u64)x[rs1] * (u64)x[rs2]) >> 32; break;
	case /*DIV*/    0x00c: 
		if(x[rs2]) x[rd] = (i32)x[rs1] / (i32)x[rs2]; 
		else x[rd] = -1; 
		break;
	case /*DIVU*/   0x00d: 
		if(x[rs2]) x[rd] = x[rs1] / x[rs2]; 
		else x[rd] = 0xffffffff; 
		break;
	case /*REM*/    0x00e: 
		if(x[rs2]) x[rd] = (i32)x[rs1] % (i32)x[rs2]; 
		else x[rd] = x[rs1];
		break;
	case /*REMU*/   0x00f: 
		if(x[rs2]) x[rd] = x[rs1] % x[rs2]; 
		else x[rd] = x[rs1];
		break;
	#endif
	}
}	

////////////////////////////////////////////////////////////////////////////////

void tinyriscv_init(tinyriscv_core* core){
	core->x[2] = tinyriscv_MEM_OFFSET + core->mem_size;
	core->pc = tinyriscv_MEM_OFFSET;
}

////////////////////////////////////////////////////////////////////////////////

u8 tinyriscv_valid_step(const tinyriscv_core* core){
	if(!MEM(32, core->mem, core->pc)) return 0;
	if(core->pc >= core->mem_size + tinyriscv_MEM_OFFSET) return 0;
	return 1;
}

////////////////////////////////////////////////////////////////////////////////

void tinyriscv_step(tinyriscv_core* core){
	//fetch
	const u32 inst = MEM(32, core->mem, core->pc);
	core->pc += 4;

	//decode
	#define opcode (inst & 0x7f)
	#define funct3 (inst >> 12 & 0x7)
	#define funct7 (inst >> 25 & 0x7f)
	#define rd (inst >> 7 & 0x1f)
	#define rs1 (inst >> 15 & 0x1f)
	#define rs2 (inst >> 20 & 0x1f)
	#define imm_i ((i32)inst >> 20)
	#define imm_s ((imm_i & 0xffffffe0) | rd)

	#define imm_b (((i32)(inst & 0x80000000) >> 19) | \
		((inst & 0x80) << 4) | (inst >> 20 & 0x7e0) | (inst >> 7 & 0x1e))

	#define imm_j (((i32)(inst & 0x80000000) >> 11) | (inst & 0xff000) | \
        (inst >> 9 & 0x800) | (inst >> 20 & 0x7fe))

	//execute
	core->x[0] = 0;

	switch(opcode){
	case /*LUI*/    0x37: core->x[rd] = inst & 0xfffff000; break;
	case /*AUIPC*/  0x17: 
		core->x[rd] = core->pc - 4 + (inst & 0xfffff000); 
		break;
	case /*JAL*/    0x6f: core->x[rd] = core->pc; core->pc += imm_j - 4; break;
	case /*JALR*/   0x67: 
		const u32 ret_addr = core->pc;
		core->pc = (core->x[rs1] + (i32)imm_i) & 0xfffffffe;
		core->x[rd] = ret_addr;
		break;
	case /*B-type*/ 0x63: 
		tinyriscv_B_type(funct3, core->x, &core->pc, rs1, rs2, imm_b); 
		break;
	case /*L-type*/ 0x03: 
		tinyriscv_L_type(funct3, core->x, core->mem, rd, rs1, imm_i); 
		break;
	case /*S-type*/ 0x23: 
		tinyriscv_S_type(funct3, core->x, core->mem, rs1, rs2, imm_s);
		break;
	case /*I-type*/ 0x13: 
		tinyriscv_I_type(funct3, funct7, core->x, rd, rs1, imm_i, rs2); 
		break;
	case /*R-type*/ 0x33: 
		tinyriscv_R_type(funct3, funct7, core->x, rd, rs1, rs2); 
		break;
	case /*FENCE*/  0x0f: break;
	}
}

////////////////////////////////////////////////////////////////////////////////

#endif //TINYRISCV_H