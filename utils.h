#pragma once

#include <cstdio>
#include<stdint.h>


/// utility functions for extracting specific parts of the Chip-8 opcodes
/// as they typically fall in one of the following types:
///     0[nnn] where nnn is a 12-bit memory address
///     00[kk] where kk (lower byte) is an 8-bit value
///     0[x][y]0 where x and y both are 4-bit values

inline uint16_t op_address(uint16_t opcode){
	return opcode & 0x0fff;
}
inline uint8_t op_x(uint16_t opcode){
	return (opcode & 0x0f00) >> 8;
}
inline uint8_t op_y(uint16_t opcode){
	return (opcode & 0x00f0) >> 4;
}
inline uint8_t op_lower(uint16_t opcode){
	return opcode & 0x00ff;
}



