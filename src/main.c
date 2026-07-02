#include <stdio.h>
#include <string.h>
#include "../include/chip8.h"

void initialize(Chip8* chip8) {
    memset(chip8->memory, 0, sizeof(chip8->memory));
    memset(chip8->V, 0, sizeof(chip8->V));
    chip8->I = 0;
    chip8->pc = 0x200;
}

// Loads a ROM file into the CHIP-8 memory
int load_rom(Chip8* chip8, const char* filename) {
    FILE* rom = fopen(filename, "rb"); // Open in "Read Binary" mode
    if (!rom) return 0; // File failed to open

    // Read the file into memory starting at 0x200
    fread(&chip8->memory[0x200], 1, 4096 - 0x200, rom);
    
    fclose(rom);
    return 1;
}

int main() {
    Chip8 myChip8;
    initialize(&myChip8);

    if (!load_rom(&myChip8, "roms/test.ch8")) {
        printf("Failed to load ROM.\n");
        return 1;
    }

    // This is our emulator's "heartbeat"
    while (1) {
        // 1. FETCH
        uint16_t opcode = myChip8.memory[myChip8.pc] << 8 | myChip8.memory[myChip8.pc + 1];
        
        // 2. DECODE
        switch (opcode & 0xF000) { 
            case 0x0000:
                printf("System operation (0x0000)\n");
                break;
            case 0x1000:
                printf("Jump to address (0x1000)\n");
                break;
            case 0xA000:
                printf("Set index register I (0xA000)\n");
                break;
            default:
                printf("Unknown opcode: 0x%X\n", opcode);
                break;
        }

        // 3. EXECUTE (Coming soon)
        
        // Break for now to avoid an infinite loop
        break; 
    } // End of while loop

    return 0;
}