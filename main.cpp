#include <cstdint>
#include<cstdio>

#include "chip8.h"
#include "chip8_debug.h"


/// TODO
/// - implement all opcode cases (debug)
/// - add font characters
/// - Read/Run modes for this emulator
/// - Read whole rom file




int main(int argc, char *argv[]){
	printf("Chip-8 emulator \n");
    
    Chip8* chip8 = new Chip8();

    // handle error while reading the source file
    if(load_file(argv[1], chip8, 64) != 0){
        return 0;
    }

    // read first 64 bytes of chip-8 memory 
	uint16_t i=0;
	while(i < 64){

        // print the addres of current instruction address and instruction as pseudo-assembly
		printf("%04x:\t", i);
        uint16_t opcode = chip8->memory[i] << 8 | chip8->memory[i+1];
        print_instruction(opcode);
		
        i+=2;
	}
}
