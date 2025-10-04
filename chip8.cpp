#include "chip8.h"
#include <cstdio>
#include <filesystem>
int load_file(char* file_path, Chip8* chip8){
    // might be read in little endian instead of big endian
	FILE* file = fopen(file_path, "rb");
	if (file == nullptr){
		printf("Provided source file could not be opened \n");
		return 1;
	}

    auto path = std::filesystem::path(file_path);

    uint16_t byte_size = std::filesystem::file_size(path);   
    printf("size %d \n", byte_size);
    for(int i=0; i < byte_size; i+=2){
        uint16_t tmp = 0;
        int read_bytes = fread(&tmp, sizeof(uint8_t) * 2, 1, file);

        // writing the 2 bytes in reverse order to convert from little to big endian
        chip8->memory[i] = tmp;
        chip8->memory[i+1] = tmp >> 8;

    }
    
    return 0;
}
