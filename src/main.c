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

void add(tinyrv_cpu* cpu, ){

	reg_file[rd] = reg_file[rs1] + reg_file[rs2];

}

void R_type(){

	switch(func3){
	case 0x0: // ADD/SUB
		if(func7 == 0x00) reg_file[rd] = reg_file[rs1] + reg_file[rs2];
		else if(func7 == 0x20) reg_file[rd] = reg_file[rs1] - reg_file[rs2];
		break;

	case /*SLL*/ 0x1: regs[rd] = regs[rs1] << (regs[rs2] & 0b11111); break;
	case /*SLT*/ 0x2: regs[rd] = (int32_t)regs[rs1] < (int32_t)regs[rs2]; break;
	case /*SLTU*/ 0x3: regs[rd] = regs[rs1] < regs[rs2]; break;
	case /*XOR*/ 0x4: regs[rd] = regs[rs1] ^ regs[rs2]; break;
	}

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

	switch(func3){

	}


	switch(opcode){
	case LUI:



	case R_type:{
			switch(func3){
			case addsub:
				switch(func7){

				}
			}
		} 		
	}
}

int main(){
	tinyrv_cpu cpu;
	cpu.pc = MEMORY_OFFSET;
}
