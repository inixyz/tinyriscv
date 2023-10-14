#ifndef TINYRISCV_H
#define TINYRISCV_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;

void tinyriscv_step(){

	switch(opcode){
	case /*R-type*/ 0x33:
		switch(func3){
		case /*ADD*/ 0x0: x[rd] = x[rs1] + x[rs2]; break;
		#ifdef TINYRISCV_VERBOSE 
		printf("add rd, rs1, rs2\n");
		#endif
		}
		break;
	}
}




case ADD: x[rd] = x[rs1] + x[rs2]; break;




#endif //TINYRISCV_H