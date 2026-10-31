#include "chip8.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <cstring> // For memset
#include <raylib.h>
#include "utils.h"
#include <cmath>

// last register
#define VF 15

Chip8::Chip8(){
    memset(memory, 0, MEM_SIZE);
    memcpy(memory, font, 16*5);
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

    size_t read_bytes = fread(&this->memory[512], sizeof(uint8_t), byte_size, file);
    fclose(file);
    if(read_bytes != byte_size){
        printf("ERROR: could not read full ROM file\n");
        return 1;
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
                    uint8_t flag = sum > 255 ? 1 : 0;
                    this->registers[x] = sum % 256; // discard the overflow
                    this->registers[VF] = flag;
                } break;
                case 0x5: {
                    // printf("SUB (reg1: %02x reg2: %02x) \n", x, y);
                    uint8_t flag = (this->registers[x] >= this->registers[y]) ? 1 : 0;
                    this->registers[x] -= this->registers[y];
                    this->registers[VF] = flag;
                } break;
                case 0x6: {
                    // printf("SHR (reg1: %02x reg2: %02x) \n", x, y);
                    uint8_t flag = this->registers[y] & 1; // write last bit of Vx to VF
                    this->registers[x] = this->registers[y] >> 1;
                    this->registers[VF] = flag;
                } break;
                case 0x7: {
                    // printf("SUBN (reg1: %02x reg2: %02x) \n", x, y);
                    uint8_t flag = (this->registers[y] >= this->registers[x]) ? 1 : 0;
                    this->registers[x] = this->registers[y] - this->registers[x];
                    this->registers[VF] = flag;
                } break;
                case 0xe: {
                    // printf("SHL (reg1: %02x reg2: %02x) \n", x, y);
                    uint8_t flag = (this->registers[y] >> 7) & 1; // write msb of Vx to VF
                    this->registers[x] = this->registers[y] << 1;
                    this->registers[VF] = flag;
                } break;

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
            // printf("JMP to $%04x + V0\n", addr);
            this->program_counter = addr + this->registers[0] - 2;
            break;
        case 0xC:
            // printf("RND (reg: %02x value: %04x)\n", x, lower);
            this->registers[x] = (rand() % 256) & lower;
            break;
        case 0xD: {
            // chat generated opcode
            // Capture coordinates before resetting VF in case Vx or Vy is VF.
            // CHIP-8 wraps the sprite's starting coordinates into the screen,
            // but clips sprite pixels that extend past the right/bottom edges.
            const int x_start = this->registers[x] % 64;
            const int y_start = this->registers[y] % 32;
            const uint8_t height = lower & 0x0F;

            this->registers[VF] = 0;

            for (int row = 0; row < height; ++row) {
                int screen_y = y_start + row;
                if (screen_y >= 32) {
                    if (this->wrap_sprites) {
                        screen_y %= 32;
                    } else {
                        continue;
                    }
                }

                const uint32_t sprite_address =
                    static_cast<uint32_t>(this->index_register) + row;
                if (sprite_address >= MEM_SIZE) {
                    break;
                }

                const uint8_t sprite = this->memory[sprite_address];
                for (int col = 0; col < 8; ++col) {
                    if ((sprite & (1u << (7 - col))) == 0) {
                        continue;
                    }

                    int screen_x = x_start + col;
                    if (screen_x >= 64) {
                        if (this->wrap_sprites) {
                            screen_x %= 64;
                        } else {
                            continue;
                        }
                    }

                    const uint8_t pixel_mask = 1u << (7 - (screen_x % 8));
                    uint8_t& screen_byte = this->vram[screen_y][screen_x / 8];
                    if (screen_byte & pixel_mask) {
                        this->registers[VF] = 1;
                    }
                    screen_byte ^= pixel_mask;
                }
            }

            this->requires_redraw = true;
            break;
        }
        case 0xE:
            switch (op_lower(opcode)) {
                case 0x9E:
                    // printf("SKIP (reg: %02x) \n", x);
                    if(IsKeyDown(keyMap[this->registers[x]])) program_counter += 2;
                    break;
                case 0xA1:
                    // printf("SKIP NOT (reg: %02x) \n", x);
                    if(!IsKeyDown(keyMap[this->registers[x]])) program_counter += 2;
                    break;
                default:
                    printf("ERROR: illegal value in opcode 0xE???\n");
                    break;
            }
            break;
        case 0xF:
            switch (op_lower(opcode)) {
                case 0x07:
                    // printf("LD (reg: %02x) <- DT \n", x);
                    this->registers[x] = this->delay_timer;
                    break;
                case 0x0A: {
                    // printf("WAIT FOR KEY (reg: %02x) \n", x);

                    int pressed_key_index = -1;
                    for(int i=0; i<16; i++){
                        if(IsKeyReleased( keyMap[i])){
                            pressed_key_index = i;
                            break;
                        }
                    }

                    // if we didn't detect a key press this instruction is repeated
                    if(pressed_key_index < 0) {
                        program_counter -= 2;
                    }else{
                        // printf("Detected press \n");
                        this->registers[x] = pressed_key_index;
                    }
                } break;
                case 0x15:
                    // printf("LD (reg: %02x) -> DT \n", x);
                    this->delay_timer = this->registers[x];
                    break;
                case 0x18:
                    // printf("LD (reg: %02x) -> ST \n", x);
                    this->sound_timer = this->registers[x];
                    break;
                case 0x1E: //
                    // printf("ADD I + (reg: %02x) \n", x);
                    this->index_register += this->registers[x];
                    break;
                case 0x29:
                    // printf("SET I = SPRITE (reg: %02x) \n", x);
                    // The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx
                    // Each sprite takes 5 bytes
                    // We assume they're stored starting at mem addres 0x0

                    this->index_register = 5 * this->registers[x];
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
                        this->memory[this->index_register] = this->registers[i];
                        this->index_register++;
                    }
                    break;
                case 0x65:
                    // printf("READ into registers up to (reg: %02x) \n", x);
                    for(int i=0; i<=x; i++){
                        this->registers[i] = this->memory[this->index_register];
                        this->index_register++;
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
