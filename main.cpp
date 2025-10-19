#include <chrono>
#include <cstdint>
#include<cstdio>
#include <cstring>
#include <ctime>

#include "chip8.h"
#include "chip8_debug.h"

#include <thread>
#include <time.h>
#include <cmath>

/// TODO
/// - add font characters
/// - Read/Run modes for this emulator
/// - Read whole rom file

const uint16_t CLOCK_RATE = 30; // in Hz

void print_program(Chip8* chip8){
	uint16_t i=512;
	while(i < MEM_SIZE){
        // Clock
        timespec t;
        t.tv_sec = 0;
        t.tv_nsec = std::floor(1.0/CLOCK_RATE * 1000 * 1000 * 1000);
        nanosleep(&t, NULL);

        // print the addres of current instruction address and instruction as pseudo-assembly
		printf("%04x:\t", i);
        uint16_t opcode = chip8->memory[i] << 8 | chip8->memory[i + 1];
        print_instruction(opcode);
		
        i+=2;
	}
}

void draw_screen(uint8_t vram[32][8]){
    const char on_char = 'o';
    const char off_char = ' ';
    printf("Drawing screen\n");
    for(int y = 0; y < 32; y++){
        for(int byte_i = 0; byte_i < 8; byte_i++){
            for(int i = 7; i >= 0; i--){
                bool pixel_on = vram[y][byte_i] >> i & 1;
                if(pixel_on){
                    printf("%c", on_char);
                }
                else{
                    printf("%c", off_char);
                }
            }
        }
        printf("\n");
    }
}

void main_loop(Chip8* chip8){
    while(true){
        // Clock
        timespec t;
        t.tv_sec = 0;
        t.tv_nsec = std::floor(1.0/CLOCK_RATE * 1000 * 1000 * 1000);
        nanosleep(&t, NULL);

        // CPU Cycle
       
        // 1. fetch instruction
        uint16_t pc = chip8->program_counter;
        uint16_t opcode = chip8->memory[pc] << 8 | chip8->memory[pc+1];
        // printf("instruction %d: \n", pc);
        // printf("%04x \n", opcode);
        
        // 2. execute instruction
        chip8->execute_instruction(opcode);

        if(chip8->requires_redraw){
            //TODO: draw data from vram
            draw_screen(chip8->vram);
            //std::this_thread::sleep_for(std::chrono::seconds(2));
            chip8->requires_redraw = false;
        }
        
        // 3. increment program counter
        chip8->program_counter += 2;
    }
}

int main(int argc, char *argv[]){
	printf("Chip-8 emulator \n");
    
    Chip8* chip8 = new Chip8();

    // handle error while reading the source file
    if(chip8->load_rom(argv[1]) != 0){
        return 0;
    }
    main_loop(chip8);
    //print_program(chip8);
    delete chip8;
}
