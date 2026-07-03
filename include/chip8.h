#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

typedef struct {
    uint8_t memory[4096];
    uint8_t V[16];
    uint16_t I;
    uint16_t pc;
    uint16_t stack[16]; // Ensure this is exactly like this
    uint8_t sp; // Ensure this is exactly like this
    uint8_t gfx[64 * 32]; // 64x32 display buffer
} Chip8;

#endif