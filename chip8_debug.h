#pragma once
#include<stdint.h>
#include "utils.h"
#include<cstdio>

//TODO: remove inlining
inline void print_instruction(uint16_t input){
    // have to be initialized before switch for compiler reasons
    uint16_t addr = op_address(input);
    uint8_t x = op_x(input);
    uint8_t y = op_y(input);
    uint8_t lower = op_lower(input);
    switch (input >> 12) {
        case 0x0:
            switch ((uint8_t)input) {
                case 0xe0:
                    printf("CLS \n");
                    break;
                case 0xee:
                    printf("RET \n");
                    break;
            }
            break;
        case 0x1:
            printf("GOTO %03x \n", addr);
            break;
        case 0x2:
            printf("CALL %03x \n", addr);
            break;
        case 0x3:
            printf("Skip EQ (reg: %02x val: %04x) \n", x, lower);
            break;
        case 0x4:
            printf("Skip NEQ (reg: %02x val: %04x) \n", x, lower);
            break;
        case 0x5:
            printf("Skip Reg EQ (reg1: %02x reg2: %02x) \n", x, y);
            break;
        case 0x6:
            printf("MOV (reg: %02x val: %04x) \n", x, lower);
            break;
        case 0x7:
            printf("ADD (reg: %02x val: %04x) \n", x, lower);
            break;
        case 0x8:
            switch (op_lower(input) & 0x0f) {
                case 0x0:
                    printf("MOV (reg1: %02x reg2: %02x) \n", x, y);
                    break;
                case 0x1:
                    printf("OR (reg1: %02x reg2: %02x) \n", x, y);
                    break;
                case 0x2:
                    printf("AND (reg1: %02x reg2: %02x) \n", x, y);
                    break;
                case 0x3:
                    printf("XOR (reg1: %02x reg2: %02x) \n", x, y);
                    break;
                case 0x4:
                    printf("ADD (reg1: %02x reg2: %02x) \n", x, y);
                    break;
                case 0x5:
                    printf("SUB (reg1: %02x reg2: %02x) \n", x, y);
                    break;
                case 0x6:
                    printf("SHR (reg1: %02x reg2: %02x) \n", x, y);
                    break;
                case 0x7:
                    printf("SUBN (reg1: %02x reg2: %02x) \n", x, y);
                    break;
                case 0xe:
                    printf("SHL (reg1: %02x reg2: %02x) \n", x, y);
                    break;

                default:
                    printf("ERROR: illegal value in opcode 0x8???\n");
                    break;
            }
            break;
        case 0x9:
            printf("Skip NEQ (reg1: %02x reg2: %02x) \n", x, y);
            break;
        case 0xA:
            printf("LD $%04x to reg I\n", addr);
            break;
        case 0xB:
            printf("JMP to $%04x + V0\n", addr);
            break;
        case 0xC:
            printf("RND (reg: %02x value: %04x)\n", x, lower);
            break;
        case 0xD:
            printf("DRAW (x: %02x y: %02x n: %04x) \n", x, y, lower & 0x0f);
            break;
        case 0xE:
            switch (op_lower(input)) {
                case 0x9E:
                    printf("SKIP (reg: %02x) \n", x);
                    break;
                case 0xA1:
                    printf("SKIP NOT (reg: %02x) \n", x);
                    break;
                default:
                    printf("ERROR: illegal value in opcode 0xE???\n");
                    break;
            }
            break;
        case 0xF:
            switch (op_lower(input)) {
                case 0x07:
                    printf("LD (reg: %02x) <- DT \n", x);
                    break;
                case 0x0A:
                    printf("WAIT FOR KEY (reg: %02x) \n", x);
                    break;
                case 0x15:
                    printf("LD (reg: %02x) -> DT \n", x);
                    break;
                case 0x18:
                    printf("LD (reg: %02x) -> ST \n", x);
                    break;
                case 0x1E:
                    printf("ADD I + (reg: %02x) \n", x);
                    break;
                case 0x29:
                    printf("SET I = SPRITE (reg: %02x) \n", x);
                    break;
                case 0x33:
                    printf("STORE BCD of (reg: %02x) \n", x);
                    break;
                case 0x55:
                    printf("STORE registers up to (reg: %02x) \n", x);
                    break;
                case 0x65:
                    printf("READ into registers up to (reg: %02x) \n", x);
                    break;
                default:
                    printf("ERROR: illegal value in opcode %04x \n", input);
                    break;
            }
            break;
        default:
            printf("Bruh - invalid instruction\n");
            break;
    }
}
