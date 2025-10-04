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
            printf("GOTO %04x \n", addr);
            break;
        case 0x2:
            printf("CALL %04x \n", addr);
            break;
        case 0x3:
            printf("Skip EQ (reg: %04x val: %04x) \n", x, lower);
            break;
        case 0x4:
            printf("Skip NEQ (reg: %04x val: %04x) \n", x, lower);
            break;
        case 0x5:
            printf("Skip Reg EQ (reg1: %04x reg2: %04x) \n", x, y);
            break;
        case 0x6:
            printf("MOV (reg: %04x val: %04x) \n", x, lower);
            break;
        case 0x7:
            printf("ADD (reg: %04x val: %04x) \n", x, lower);
            break;
        case 0x8:
            switch (op_lower(input) & 0x0f) {
                case 0x0:
                    printf("MOV (reg1: %04x reg2: %04x) \n", x, y);
                    break;
                case 0x1:
                    printf("OR (reg1: %04x reg2: %04x) \n", x, y);
                    break;
                case 0x2:
                    printf("AND (reg1: %04x reg2: %04x) \n", x, y);
                    break;
                case 0x3:
                    printf("XOR (reg1: %04x reg2: %04x) \n", x, y);
                    break;
                case 0x4:
                    printf("ADD (reg1: %04x reg2: %04x) \n", x, y);
                    break;
                case 0x5:
                    printf("SUB (reg1: %04x reg2: %04x) \n", x, y);
                    break;
                case 0x6:
                    printf("SHR (reg1: %04x reg2: %04x) \n", x, y);
                    break;
                case 0x7:
                    printf("SUBN (reg1: %04x reg2: %04x) \n", x, y);
                    break;
                case 0xe:
                    printf("SHL (reg1: %04x reg2: %04x) \n", x, y);
                    break;

                default:
                    printf("ERROR: illegal value in opcode 0x8???\n");
                    break;
            }
            break;
        case 0x9:
            printf("Skip NEQ (reg1: %04x reg2: %04x) \n", x, y);
            break;
        case 0xd:
            printf("DRAW (x: %04x y: %04x n: %04x) \n", x, y, lower & 0x0f);
            break;

        case 0xa:
            printf("MEM $%04x \n", addr);
            break;
        default:
            printf("Bruh - invalid instruction\n");
            break;
    }
}
