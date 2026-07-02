#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h> // This gives us standard-sized integers like uint8_t

typedef struct {
    uint8_t memory[4096];      // 4KB of RAM
    uint8_t V[16];             // 16 8-bit registers (V0-VF)
    uint16_t I;                // Index register (16-bit)
    uint16_t pc;               // Program counter
} Chip8;

#endif