#include <stdio.h>
#include <string.h>
#include "../include/chip8.h"

void initialize(Chip8* chip8) {
    memset(chip8->memory, 0, sizeof(chip8->memory));
    memset(chip8->V, 0, sizeof(chip8->V));
    memset(chip8->stack, 0, sizeof(chip8->stack)); // Clear the stack
    memset(chip8->gfx, 0, sizeof(chip8->gfx));     // Clear the display buffer
    chip8->I = 0;
    chip8->pc = 0x200;
    chip8->sp = 0;                                 // Reset stack pointer
}

int load_rom(Chip8* chip8, const char* filename) {
    FILE* rom = fopen(filename, "rb");
    if (!rom) return 0;
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

    while (1) {
        // 1. FETCH
        uint16_t opcode = myChip8.memory[myChip8.pc] << 8 | myChip8.memory[myChip8.pc + 1];
        
        // 2. DECODE
        switch (opcode & 0xF000) { 
            case 0x0000:
                // Nested switch for specific system operations
                switch (opcode & 0x00FF) {
                    case 0x00E0:
                        printf("Clear Screen (0x00E0)\n");
                        memset(myChip8.gfx, 0, sizeof(myChip8.gfx));
                        break;
                    case 0x00EE:
                        printf("Return from Subroutine (0x00EE)\n");
                        myChip8.sp--;
                        myChip8.pc = myChip8.stack[myChip8.sp];
                        break;
                    default:
                        printf("Unknown System Operation: 0x%X\n", opcode);
                        break;
                }
                break;
            case 0x1000:
                printf("Jump to address (0x1000)\n");
                break;
            case 0xA000:
                printf("Set index register I (0xA000)\n");
                break;
            case 0x2000: // Call subroutine
                myChip8.stack[myChip8.sp] = myChip8.pc;
                myChip8.sp++;
                myChip8.pc = opcode & 0x0FFF; 
                printf("Jump to Subroutine: 0x%X\n", myChip8.pc);
                break;    
            default:
                printf("Unknown opcode: 0x%X\n", opcode);
                break;
        }

        break; // Stop after first cycle for now
    } 

    return 0;
}