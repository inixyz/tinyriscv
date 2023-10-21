#include <stdint.h>

int main(){
	uint32_t* ptr = 0x800000F0;
	
	*ptr = 1;
	while(*ptr) (*ptr)++;
}