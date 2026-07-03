#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void chip8_init(chip8* cpu) {
    memset(cpu, 0, sizeof(chip8));
    cpu->pc = 0x200;
    cpu->sp = 0;
    cpu->I = 0;
    
    uint8_t fontset[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, 0x20, 0x60, 0x20, 0x20, 0x70, 0xF0, 0x10, 0xF0, 0x80, 0xF0, 0xF0,
        0x10, 0xF0, 0x10, 0xF0, 0x90, 0x90, 0xF0, 0x10, 0x10, 0xF0, 0x80, 0xF0, 0x10, 0xF0, 0xF0, 0x80,
        0xF0, 0x90, 0xF0, 0xF0, 0x10, 0x20, 0x40, 0x40, 0xF0, 0x90, 0xF0, 0x90, 0xF0, 0xF0, 0x90, 0xF0,
        0x10, 0xF0, 0xF0, 0x90, 0xF0, 0x90, 0x90, 0xE0, 0x90, 0xE0, 0x90, 0xE0, 0xF0, 0x80, 0x80, 0x80,
        0xF0, 0xE0, 0x90, 0x90, 0x90, 0xE0, 0xF0, 0x80, 0xF0, 0x80, 0xF0, 0xF0, 0x80, 0xF0, 0x80, 0x80
    };
    for(int i = 0; i < 80; i++) cpu->memory[i] = fontset[i];
    srand(time(NULL));
}

void chip8_load_rom(chip8* cpu, const char* filename) {
    FILE* rom = fopen(filename, "rb");
    if (!rom) { printf("Error: Could not open ROM\n"); exit(1); }
    size_t result = fread(&cpu->memory[0x200], 1, 4096 - 0x200, rom);
    if (result == 0) printf("Warning: ROM read failed.\n");
    fclose(rom);
}

void chip8_emulate_cycle(chip8* cpu) {
    uint16_t opcode = (cpu->memory[cpu->pc] << 8) | cpu->memory[cpu->pc + 1];
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    switch (opcode & 0xF000) {
        case 0x0000:
            if ((opcode & 0x00FF) == 0x00E0) { memset(cpu->gfx, 0, 2048); cpu->draw_flag = true; }
            else if ((opcode & 0x00FF) == 0x00EE) cpu->pc = cpu->stack[--cpu->sp];
            cpu->pc += 2; break;
        case 0x1000: cpu->pc = (opcode & 0x0FFF); break;
        case 0x2000: cpu->stack[cpu->sp++] = cpu->pc; cpu->pc = (opcode & 0x0FFF); break;
        case 0x3000: cpu->pc += (cpu->V[x] == (opcode & 0x00FF)) ? 4 : 2; break;
        case 0x4000: cpu->pc += (cpu->V[x] != (opcode & 0x00FF)) ? 4 : 2; break;
        case 0x6000: cpu->V[x] = (opcode & 0x00FF); cpu->pc += 2; break;
        case 0x7000: cpu->V[x] += (opcode & 0x00FF); cpu->pc += 2; break;
        case 0x8000:
            if ((opcode & 0x000F) == 0x0) cpu->V[x] = cpu->V[y];
            else if ((opcode & 0x000F) == 0x4) { uint16_t sum = cpu->V[x] + cpu->V[y]; cpu->V[0xF] = (sum > 0xFF); cpu->V[x] = sum & 0xFF; }
            else if ((opcode & 0x000F) == 0x5) { cpu->V[0xF] = (cpu->V[x] > cpu->V[y]); cpu->V[x] -= cpu->V[y]; }
            cpu->pc += 2; break;
        case 0xA000: cpu->I = (opcode & 0x0FFF); cpu->pc += 2; break;
        case 0xC000: cpu->V[x] = (rand() % 256) & (opcode & 0x00FF); cpu->pc += 2; break;
        case 0xD000: {
            uint16_t xPos = cpu->V[x] % 64; uint16_t yPos = cpu->V[y] % 32;
            cpu->V[0xF] = 0;
            for (int r = 0; r < (opcode & 0x000F); r++) {
                uint8_t pixel = cpu->memory[cpu->I + r];
                for (int c = 0; c < 8; c++) {
                    if ((pixel & (0x80 >> c))) {
                        if (cpu->gfx[xPos + c + (yPos + r) * 64]) cpu->V[0xF] = 1;
                        cpu->gfx[xPos + c + (yPos + r) * 64] ^= 1;
                    }
                }
            }
            cpu->draw_flag = true; cpu->pc += 2;
        } break;
        case 0xE000: if ((opcode & 0x00FF) == 0xA1) cpu->pc += (!cpu->key[cpu->V[x]]) ? 4 : 2; else cpu->pc += 2; break;
        case 0xF000:
            if ((opcode & 0x00FF) == 0x07) cpu->V[x] = cpu->delay_timer;
            else if ((opcode & 0x00FF) == 0x15) cpu->delay_timer = cpu->V[x];
            else if ((opcode & 0x00FF) == 0x18) cpu->sound_timer = cpu->V[x];
            else if ((opcode & 0x00FF) == 0x29) cpu->I = cpu->V[x] * 5;
            else if ((opcode & 0x00FF) == 0x33) { cpu->memory[cpu->I] = cpu->V[x]/100; cpu->memory[cpu->I+1] = (cpu->V[x]/10)%10; cpu->memory[cpu->I+2] = cpu->V[x]%10; }
            else if ((opcode & 0x00FF) == 0x65) for(int i=0; i<=x; i++) cpu->V[i] = cpu->memory[cpu->I+i];
            cpu->pc += 2; break;
        default: cpu->pc += 2; break;
    }
}