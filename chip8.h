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
	uint16_t program_counter = 0;
	uint8_t stack_pointer = 0;

};

int load_file(char* file_path, Chip8* chip8);
