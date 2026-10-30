#include "chip8.h"
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <cstring> // For memset
#include "utils.h"

// last register
#define VF 15

Chip8::Chip8(){
    memset(memory, 0, MEM_SIZE);
    memset(vram, 0, 8*32);
    memset(registers, 0, 16);
    memset(stack, 0, 16);
    program_counter = 512;
}

int Chip8::load_rom(char* file_path){
    // might be read in little endian instead of big endian
	FILE* file = fopen(file_path, "rb");
	if (file == nullptr){
		printf("Provided source file could not be opened \n");
		return 1;
	}

    auto path = std::filesystem::path(file_path);

    uint16_t byte_size = std::filesystem::file_size(path);

    if(byte_size > MEM_SIZE - 512){
        printf("ERROR: provided rom file is too large to be stored in memory");
        return 1;
    }

    for(int i=0; i < byte_size; i+=2){
        uint16_t tmp = 0;
        int read_bytes = fread(&tmp, sizeof(uint8_t) * 2, 1, file);

        // writing the 2 bytes in reverse order to convert from little to big endian
        this->memory[512 + i] = tmp;
        this->memory[512 + i + 1] = tmp >> 8;
    }
    
    return 0;
}

uint8_t Chip8::execute_instruction(uint16_t opcode){
    // have to be initialized before switch for compiler reasons
    uint16_t addr = op_address(opcode);
    uint8_t x = op_x(opcode);
    uint8_t y = op_y(opcode);
    uint8_t lower = op_lower(opcode);
    switch (opcode >> 12) {
        case 0x0:
            switch ((uint8_t)opcode) {
                case 0xe0:
                    //CLS
                    memset(this->vram, 0 , sizeof(uint8_t) * 32*8);
                    this->requires_redraw = true;
                    break;
                case 0xee:
                    // printf("RET \n");
                    // ret - load return address from the stack then set PC to that value
                    this->program_counter = this->stack[this->stack_pointer-1];
                    this->stack_pointer--;
                    break;
            }
            break;
        case 0x1:
            //printf("GOTO %03x \n", addr);
            if(addr < 512 || addr >= 4096) return 1; // ERROR: reading outside memory
            this->program_counter = addr - 2; // PC is automaticall incremented by 2 after this instruction so we have to offset that
            break;
        case 0x2:
            // printf("2nnn \n");
            // 2nnn call addr
            this->stack_pointer++;
            this->stack[this->stack_pointer-1]= this->program_counter;
            this->program_counter = addr -2;
            break;
        case 0x3:
            // printf("Skip EQ (reg: %02x val: %04x) \n", x, lower);

            // 3xnn: skip the next instruction if Vx == nn.
            if (this->registers[x] == lower) {
                this->program_counter += 2;
            }
            break;
        case 0x4:
            // 4xnn: skip the next instruction if Vx != nn.
            if (this->registers[x] != lower) {
                this->program_counter += 2;
            }
            break;
        case 0x5:
            // 5xy0: skip the next instruction if Vx == Vy.
            if ((opcode & 0x00F) == 0 && this->registers[x] == this->registers[y]) {
                this->program_counter += 2;
            }
            break;
        case 0x6:
            // printf("MOV (reg: %02x val: %04x) \n", x, lower);
            this->registers[x] = lower;
            break;
        case 0x7:
            //printf("ADD (reg: %02x val: %04x) \n", x, lower);
            this->registers[x] += lower;
            break;
        case 0x8:
            switch (op_lower(opcode) & 0x0f) {
                case 0x0:
                    // printf("MOV (reg1: %02x reg2: %02x) \n", x, y);
                    this->registers[x] = this->registers[y];
                    break;
                case 0x1:
                    // printf("OR (reg1: %02x reg2: %02x) \n", x, y);
                    this->registers[x] |= this->registers[y];
                    break;
                case 0x2:
                    // printf("AND (reg1: %02x reg2: %02x) \n", x, y);
                    this->registers[x] &= this->registers[y];
                    break;
                case 0x3:
                    // printf("XOR (reg1: %02x reg2: %02x) \n", x, y);
                    this->registers[x] ^= this->registers[y];
                    break;
                case 0x4: {
                    //printf("ADD (reg1: %02x reg2: %02x) \n", x, y);
                    // temporarily store in a 32bit variable to detect overflow
                    uint16_t sum = this->registers[x] + this->registers[y];
                    // overflow flag
                    this->registers[VF] = sum > 255 ? 1 : 0;
                    this->registers[x] = sum % 256; // discard the overflow
                } break;
                case 0x5:
                    // printf("SUB (reg1: %02x reg2: %02x) \n", x, y);
                    this->registers[VF] = this->registers[y] > this->registers[x] ? 1 : 0;
                    this->registers[x] -= this->registers[y];
                    break;
                case 0x6:
                    // printf("SHR (reg1: %02x reg2: %02x) \n", x, y);
                    this->registers[VF] = this->registers[x] & 1; // write last bit of Vx to VF
                    this->registers[x] = this->registers[x] >> 1;
                    break;
                case 0x7:
                    // printf("SUBN (reg1: %02x reg2: %02x) \n", x, y);
                    this->registers[VF] = this->registers[y] > this->registers[x] ? 1 : 0;
                    this->registers[x] = this->registers[y] - this->registers[x];
                    break;
                case 0xe:
                    // printf("SHL (reg1: %02x reg2: %02x) \n", x, y);
                    this->registers[VF] = (this->registers[x] >> 15) & 1; // write msb of Vx to VF
                    this->registers[x] = this->registers[x] << 1;

                    // TODO: VF flag
                    break;

                default:
                    printf("ERROR: illegal value in opcode 0x8???\n");
                    break;
            }
            break;
        case 0x9:
            // 9xy0: skip the next instruction if Vx != Vy.
            if ((opcode & 0x00F) == 0 && this->registers[x] != this->registers[y]) {
                this->program_counter += 2;
            }
            break;
        case 0xA:
            //printf("LD $%04x to reg I\n", addr);
            this->index_register = addr;
            break;
        case 0xB:
            printf("JMP to $%04x + V0\n", addr);
            break;
        case 0xC:
            printf("RND (reg: %02x value: %04x)\n", x, lower);
            break;
        case 0xD:
            // printf("DRAW (x: %02x y: %02x n: %04x) \n", this->registers[x], this->registers[y], lower & 0x0f);
            //TODO: need to handle offset lesser than 1 byte
            {
                uint8_t x_val = this->registers[x]; 
                uint8_t y_val = this->registers[y];
                const uint8_t offset = x_val % 8;

                for(int i=0; i < (lower & 0x0f); i++){
                    vram[y_val+i][x_val/8] ^= this->memory[this->index_register+i] >> offset;
                    
                    if(offset != 0)
                        vram[y_val+i][x_val/8 + 1] ^= this->memory[this->index_register+i] << (8 - offset);
                }
                this->requires_redraw = true;
                
            }
            break;
        case 0xE:
            switch (op_lower(opcode)) {
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
            switch (op_lower(opcode)) {
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
                case 0x1E: //
                    // printf("ADD I + (reg: %02x) \n", x);
                    this->index_register += this->registers[x];
                    break;
                case 0x29:
                    printf("SET I = SPRITE (reg: %02x) \n", x);
                    break;
                case 0x33: {
                    // printf("STORE BCD of (reg: %02x) \n", x);
                    uint8_t tmp_x = this->registers[x];
                    uint8_t ones = tmp_x % 10;
                    tmp_x -= ones;
                    tmp_x /= 10;
                    uint8_t tens = tmp_x % 10;
                    tmp_x -= tens;
                    tmp_x /= 10;
                    uint8_t hundreds = tmp_x % 10;
                    this->memory[this->index_register] = hundreds;
                    this->memory[this->index_register + 1] = tens;
                    this->memory[this->index_register + 2] = ones;
                } break;
                case 0x55:
                    // printf("STORE registers up to (reg: %02x) \n", x);
                    for(int i=0; i<=x; i++){
                        this->memory[this->index_register + i] = this->registers[i];
                    }
                    break;
                case 0x65:
                    // printf("READ into registers up to (reg: %02x) \n", x);
                    for(int i=0; i<=x; i++){
                        this->registers[i] = this->memory[this->index_register + i];
                    }
                    break;
                default:
                    printf("ERROR: illegal value in opcode %04x \n", opcode);
                    break;
            }
            break;
        default:
            printf("Bruh - invalid instruction\n");
            break;
    }
    return 0;
}
