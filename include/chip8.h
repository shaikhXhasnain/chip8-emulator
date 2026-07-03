#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

typedef struct {
    uint8_t  memory[4096];
    uint8_t  V[16];      // General purpose registers
    uint16_t I;          // Index register
    uint16_t pc;         // Program counter
    uint16_t stack[16];  // Stack
    uint8_t  sp;         // Stack pointer
    uint8_t  gfx[64 * 32]; // Graphics buffer
} Chip8;

// Function prototypes
void initialize(Chip8* chip8);
int  load_rom(Chip8* chip8, const char* filename);

#endif