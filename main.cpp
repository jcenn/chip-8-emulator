#include <cstdint>
#include<stdio.h>
#include<stdint.h>
#include<cstdio>



/// TODO
/// - implement all opcode cases (debug)
/// - add font characters
/// - Read/Run modes for this emulator
/// - Read whole rom file

typedef uint16_t mem_addr;

const uint16_t MEMORY_BYTES = 4096;

struct Chip8{
	// Accessible for programs
	uint8_t registers[16];
	uint16_t index_register;
	uint8_t delay_timer;
	uint8_t sound_timer;
	
	// Internal only
	uint8_t memory[4096];
	uint16_t stack[16];
	uint16_t program_counter;
	uint8_t stack_pointer;

};

// use a mask to extract specific bits
mem_addr op_address(uint16_t opcode){
	return opcode & 0x0fff;
}
mem_addr op_x(uint16_t opcode){
	
	return (opcode & 0x0f00) >> 8;
}
mem_addr op_y(uint16_t opcode){
	return (opcode & 0x00f0) >> 4;
}
mem_addr op_lower(uint16_t opcode){
	return opcode & 0x00ff;
}

void print_instruction(uint16_t input){
    // have to be initialized before switch for compiler reasons
    mem_addr addr = op_address(input);
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
            printf("Skip Reg NEQ (reg1: %04x reg2: %04x) \n", x, y);
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

void print_program_file(char* file_path, uint16_t byte_size){
}

int load_file(char* file_path, Chip8* chip8, uint16_t byte_size){
    uint16_t bytes = MEMORY_BYTES;
    // might be read in little endian instead of big endian
	FILE* file = fopen(file_path, "rb");
	if (file == nullptr){
		printf("Ya fucked up boy \n");
		return 1;
	}

     for(int i=0; i < byte_size; i+=2){
         uint16_t tmp = 0;
         fread(&tmp, sizeof(uint8_t) * 2, 1, file);
        
         // writing in reverse order
         chip8->memory[i] = tmp;
         chip8->memory[i+1] = tmp >> 8;

     }
    
    return 0;
}

int main(int argc, char *argv[]){
	printf("Chip8 emulator \n");
    
    Chip8* chip8 = new Chip8();

	printf("%s \n", argv[1]);
	uint16_t input;
    
    load_file(argv[1], chip8, 64);

    // reads first i bytes of the memory 
	uint16_t i=0;
	while(i < 64){
		// temp variable and weird bitshift needed to convert from little endian to big endian
		// There might be a cleaner solution to this

        // print the addres of current instruction
		printf("%04x:\t", i);
        uint16_t opcode = chip8->memory[i] << 8 | chip8->memory[i+1];
        print_instruction(opcode);
		i+=2;


	}
}
