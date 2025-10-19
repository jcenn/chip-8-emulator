#pragma once

#include <cstdint>

const uint16_t MEM_SIZE = 4096;

struct Chip8{
    Chip8();
	// Accessible for programs
	uint8_t registers[16];
	uint16_t index_register = 0;
	uint8_t delay_timer = 0;
	uint8_t sound_timer = 0;
	
	// Internal only
	uint8_t memory[MEM_SIZE];
	uint16_t stack[16];
	uint16_t program_counter = 512;
	uint8_t stack_pointer = 0;
    // 64x32 screen
    uint8_t vram[32][8];
    
    bool requires_redraw = false;
    
    int load_rom(char* file_path);
    void step();
    uint8_t execute_instruction(uint16_t opcode);
};

