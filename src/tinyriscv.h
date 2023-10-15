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
	u32 x[32], pc, mem_size;
	u8* mem;
}tinyriscv_hart;

const u32 tinyriscv_MEM_OFFSET = 0x80000000;

static inline u8 load8(const u8* mem, const u32 addr){
	return mem[addr - tinyriscv_MEM_OFFSET];
}

static inline u16 load16(const u8* mem, const u32 addr){
	return *(u16*)(mem + addr - tinyriscv_MEM_OFFSET);
}

static inline u32 load32(const u8* mem, const u32 addr){
	return *(u32*)(mem + addr - tinyriscv_MEM_OFFSET);
}

static inline void store8(u8* mem, const u32 addr, const u8 val){
	mem[addr - tinyriscv_MEM_OFFSET] = val;
}

static inline void store16(u8* mem, const u32 addr, const u16 val){
	*(u16*)(mem + addr - tinyriscv_MEM_OFFSET) = val;
}

static inline void store32(u8* mem, const u32 addr, const u32 val){
	*(u32*)(mem + addr - tinyriscv_MEM_OFFSET) = val;
}

void tinyriscv_b_type(u32* pc, const u8 func3, const u32 x[32], const u8 rs1, 
	const u8 rs2, const i16 imm){

	pc -= 4;

	switch(func3){
	case /*BEQ*/  0: if(x[rs1] == x[rs2]) *pc += (i32)imm; break; 
	case /*BNE*/  1: if(x[rs1] != x[rs2]) *pc += (i32)imm; break;
	case /*BLT*/  4: if((i32)x[rs1] < (i32)x[rs2]) *pc += (i32)imm; break; 
	case /*BGE*/  5: if((i32)x[rs1] >= (i32)x[rs2]) *pc += (i32)imm; break;
	case /*BLTU*/ 6: if(x[rs1] < x[rs2]) *pc += (i32)imm; break; 
	case /*BGEU*/ 7: if(x[rs1] >= x[rs2]) *pc += (i32)imm; break;
	}
}

void tinyriscv_l_type(const u8* mem, const u8 func3, u32 x[32], const u8 rd, 
	const u8 rs1, const i16 imm){

	const u32 addr = x[rs1] + (i32)imm;

	switch(func3){
	case /*LB*/  0: x[rd] = (i32)(i8)load8(mem, addr); break;
	case /*LH*/  1: x[rd] = (i32)(i16)load16(mem, addr); break;
	case /*LW*/  2: x[rd] = load32(mem, addr); break;
	case /*LBU*/ 4: x[rd] = load8(mem, addr); break;
	case /*LHU*/ 5: x[rd] = load16(mem, addr); break;
	}
}

void tinyriscv_s_type(u8* mem, const u8 func3, const u32 x[32], const u8 rs1, 
	const u8 rs2, const i16 imm){

	const u32 addr = x[rs1] + (i32)imm;

	switch(func3){
	case /*SB*/ 0: store8(mem, addr, x[rs2]); break;
	case /*SH*/ 1: store16(mem, addr, x[rs2]); break;
	case /*SW*/ 2: store32(mem, addr, x[rs2]); break;
	}
}

void tinyriscv_i_type(const u8 func3, const u8 func7, u32 x[32], const u8 rd, 
	const u8 rs1, const i16 imm, const u8 shamt){

	#define ADDI  (x[rd] = x[rs1] + (i32)imm)
	#define SLTI  (x[rd] = (i32)x[rs1] < (i32)imm)
	#define SLTIU (x[rd] = x[rs1] < (u32)(i32)imm)
	#define XORI  (x[rd] = x[rs1] ^ (i32)imm)
	#define ORI   (x[rd] = x[rs1] | (i32)imm)
	#define ANDI  (x[rd] = x[rs1] & (i32)imm)
	#define SLLI  (x[rd] = x[rs1] << shamt)
	#define SRLI  (x[rd] = x[rs1] >> shamt)
	#define SRAI  (x[rd] = (i32)x[rs1] >> shamt)

	switch(func3){
	case 0: ADDI; break; case 1: SLLI; break;
	case 2: SLTI; break; case 3: SLTIU; break;
	case 4: XORI; break;
	case 5: func7 ? SRAI : SRLI; break;
	case 6: ORI; break; case 7: ANDI; break;
	}
}

void tinyriscv_r_type(const u8 func3, const u8 func7, u32 x[32], const u8 rd,
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

void tinyriscv_step(tinyriscv_hart* hart){
	//fetch
	const u32 inst = load32(hart->mem, hart->pc);
	hart->pc += 4;

	//decode
	#define opcode (inst & 0x7f)
	#define func3 (inst >> 12 & 0x7)
	#define func7 (inst >> 25 & 0x7f)
	#define rd (inst >> 7 & 0x1f)
	#define rs1 (inst >> 15 & 0x1f)
	#define rs2 (inst >> 20 & 0x1f)
	#define imm_i ((i32)inst >> 20)
	#define imm_s (imm_i & 0xffffffe0 | rd)

	#define imm_b (((i32)(inst & 0x80000000) >> 19) | \
		((inst & 0x80) << 4) | (inst >> 20 & 0x7e0) | (inst >> 7 & 0x1e))

	#define imm_j (((i32)(inst & 0x80000000) >> 11) | (inst & 0xff000) | \
        (inst >> 9 & 0x800) | (inst >> 20 & 0x7fe))

	//execute
	hart->x[0] = 0;

	switch(opcode){
	case /*LUI*/   0x37: hart->x[rd] = inst & 0xfffff000; break;
	case /*AUIPC*/ 0x17: hart->x[rd] = hart->pc + (inst & 0xfffff000); break;
	case /*JAL*/   0x6f: hart->x[rd] = hart->pc; hart->pc += imm_j - 4; break;
	case /*JALR*/  0x67: 
		const u32 last_pc = hart->pc;
		hart->pc = (hart->x[rs1] + (i32)imm_i) & 0xfffffffe;
		hart->x[rd] = last_pc;
		break;

	case 0x63: tinyriscv_b_type(&hart->pc, func3, hart->x, rs1, rs2, imm_b); break;
	case 0x03: tinyriscv_l_type(hart->mem, func3, hart->x, rd, rs1, imm_i); break;
	case 0x23: tinyriscv_s_type(hart->mem, func3, hart->x, rs1, rs2, imm_s); break;
	case 0x13: tinyriscv_i_type(func3, func7, hart->x, rd, rs1, imm_i, rs2); break;
	case 0x33: tinyriscv_r_type(func3, func7, hart->x, rd, rs1, rs2); break;
	}
}

void tinyriscv_init(tinyriscv_hart* hart){
	hart->x[2] = tinyriscv_MEM_OFFSET + hart->mem_size;
	hart->pc = tinyriscv_MEM_OFFSET;
}

#endif //TINYRISCV_H