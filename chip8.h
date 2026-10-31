#pragma once

#include <cstdint>
#include "raylib.h"

const uint16_t MEM_SIZE = 4096;

static const uint8_t font[] = {
// 0
0xF0,
0x90,
0x90,
0x90,
0xF0,
// 1,
0x20,
0x60,
0x20,
0x20,
0x70,
// 2,
0xF0,
0x10,
0xF0,
0x80,
0xF0,
// 3,
0xF0,
0x10,
0xF0,
0x10,
0xF0,
// 4,
0x90,
0x90,
0xF0,
0x10,
0x10,
// 5,
0xF0,
0x80,
0xF0,
0x10,
0xF0,
// 6,
0xF0,
0x80,
0xF0,
0x90,
0xF0,
// 7,
0xF0,
0x10,
0x20,
0x40,
0x40,
// 8,
0xF0,
0x90,
0xF0,
0x90,
0xF0,
// 9,
0xF0,
0x90,
0xF0,
0x10,
0xF0,
// A,
0xF0,
0x90,
0xF0,
0x90,
0x90,
// B,
0xE0,
0x90,
0xE0,
0x90,
0xE0,
// C,
0xF0,
0x80,
0x80,
0x80,
0xF0,
// D,
0xE0,
0x90,
0x90,
0x90,
0xE0,
// E,
0xF0,
0x80,
0xF0,
0x80,
0xF0,
// F,
0xF0,
0x80,
0xF0,
0x80,
0x80,
};

static const KeyboardKey keyMap[16] = {
    KeyboardKey::KEY_ZERO, KeyboardKey::KEY_ONE, KeyboardKey::KEY_TWO, KeyboardKey::KEY_THREE,
    KeyboardKey::KEY_FOUR, KeyboardKey::KEY_UP, KeyboardKey::KEY_SIX, KeyboardKey::KEY_LEFT,
    KeyboardKey::KEY_DOWN, KeyboardKey::KEY_RIGHT, KeyboardKey::KEY_A,  KeyboardKey::KEY_B,
    KeyboardKey::KEY_C, KeyboardKey::KEY_D,KeyboardKey::KEY_E, KeyboardKey::KEY_F,
};

static const uint16_t CLOCK_RATE = 8000; // in Hz

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
    bool wrap_sprites = true; // false = clip (classic CHIP-8), true = wrap (Octo / XO-CHIP)
    
    int load_rom(char* file_path);
    void step();
    uint8_t execute_instruction(uint16_t opcode);
};

