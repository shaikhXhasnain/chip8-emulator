#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t memory[4096];
    uint8_t V[16];
    uint16_t I;
    uint16_t pc;
    uint16_t stack[16];
    uint8_t sp;
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t key[16];
    uint8_t gfx[64 * 32];
    bool draw_flag;
} chip8; // <--- The typedef name must match exactly

void chip8_init(chip8* cpu);
void chip8_load_rom(chip8* cpu, const char* filename);
void chip8_emulate_cycle(chip8* cpu);

#endif