#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/chip8.h"

void initialize(Chip8* chip8) {
    memset(chip8->memory, 0, sizeof(chip8->memory));
    memset(chip8->V, 0, sizeof(chip8->V));
    memset(chip8->stack, 0, sizeof(chip8->stack));
    memset(chip8->gfx, 0, sizeof(chip8->gfx));
    chip8->I = 0;
    chip8->pc = 0x200; // CHIP-8 programs start here
    chip8->sp = 0;
}

int load_rom(Chip8* chip8, const char* filename) {
    FILE* rom = fopen(filename, "rb");
    if (!rom) return 0;
    // Load ROM into memory starting at 0x200
    size_t read = fread(&chip8->memory[0x200], 1, 4096 - 0x200, rom);
    fclose(rom);
    return (read > 0);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: ./build/chip8 <rom_path>\n");
        return 1;
    }

    Chip8 myChip8;
    initialize(&myChip8);

    if (!load_rom(&myChip8, argv[1])) {
        printf("Failed to load ROM: %s\n", argv[1]);
        return 1;
    }

    // Main Emulation Loop
    while (1) {
        // 1. FETCH
        uint16_t opcode = (myChip8.memory[myChip8.pc] << 8) | myChip8.memory[myChip8.pc + 1];
        myChip8.pc += 2; // Increment PC before execution
        
        // 2. DECODE & EXECUTE
        switch (opcode & 0xF000) {
            case 0x0000:
                if ((opcode & 0x00FF) == 0xE0) { /* Clear Screen */ }
                else if ((opcode & 0x00FF) == 0xEE) { /* Return */ myChip8.sp--; myChip8.pc = myChip8.stack[myChip8.sp]; }
                break;
            case 0x1000: // Jump 1NNN
                myChip8.pc = opcode & 0x0FFF;
                break;
            case 0x2000: // Call 2NNN
                myChip8.stack[myChip8.sp++] = myChip8.pc;
                myChip8.pc = opcode & 0x0FFF;
                break;
            case 0x7000: // ADD VX, byte
                myChip8.V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
                break;
            case 0x8000: // Math & Logic
                {
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    uint8_t y = (opcode & 0x00F0) >> 4;
                    switch (opcode & 0x000F) {
                        case 0x0: myChip8.V[x] = myChip8.V[y]; break; // LD VX, VY
                        case 0x4: // ADD VX, VY with carry
                            myChip8.V[0xF] = (myChip8.V[x] + myChip8.V[y] > 255);
                            myChip8.V[x] += myChip8.V[y];
                            break;
                    }
                }
                break;
            case 0xA000: // Set I
                myChip8.I = opcode & 0x0FFF;
                break;
            default:
                printf("Opcode not implemented: 0x%X\n", opcode);
                break;
        }
        
        // In a real game, you'd add a timer/sleep here to control CPU speed
    }
    return 0;
}