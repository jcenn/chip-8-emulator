#include <cstdint>
#include<cstdio>
#include <cstring>
#include <ctime>

#include "chip8.h"
#include "chip8_debug.h"

#include <time.h>
#include <cmath>

/// TODO
/// - implement all opcode cases (debug)
/// - add font characters
/// - Read/Run modes for this emulator
/// - Read whole rom file

const uint16_t CLOCK_RATE = 2; // in Hz


int main(int argc, char *argv[]){
	printf("Chip-8 emulator \n");
    
    Chip8* chip8 = new Chip8();
    memset(&chip8->memory, 0, MEM_SIZE);

    // handle error while reading the source file
    if(load_file(argv[1], chip8) != 0){
        return 0;
    }

    // read first 64 bytes of chip-8 memory 
	uint16_t i=0;
	while(i < 64){
        // Clock
        timespec t;
        t.tv_sec = 0;
        t.tv_nsec = std::floor(1.0/CLOCK_RATE * 1000 * 1000 * 1000);
        nanosleep(&t, NULL);

        // print the addres of current instruction address and instruction as pseudo-assembly
		printf("%04x:\t", i);
        uint16_t opcode = chip8->memory[i] << 8 | chip8->memory[i+1];
        print_instruction(opcode);
		
        i+=2;
	}
}
