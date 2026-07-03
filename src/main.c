#include <SDL2/SDL.h>
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
    chip8->pc = 0x200;
    chip8->sp = 0;
}

int load_rom(Chip8* chip8, const char* filename) {
    FILE* rom = fopen(filename, "rb");
    if (!rom) return 0;
    size_t read = fread(&chip8->memory[0x200], 1, 4096 - 0x200, rom);
    fclose(rom);
    return (read > 0);
}

int main(int argc, char* argv[]) {
    if (argc < 2) { printf("Usage: ./build/chip8 <rom_path>\n"); return 1; }

    Chip8 myChip8;
    initialize(&myChip8);
    if (!load_rom(&myChip8, argv[1])) return 1;

    // --- SDL Initialization ---
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
    uint32_t pixels[64 * 32];
    int running = 1;
    SDL_Event event;

    // Main Emulation Loop
    while (running) {
        // Event Handling
        while (SDL_PollEvent(&event)) { if (event.type == SDL_QUIT) running = 0; }

        // 1. FETCH
        uint16_t opcode = (myChip8.memory[myChip8.pc] << 8) | myChip8.memory[myChip8.pc + 1];
        myChip8.pc += 2;
        
        // 2. DECODE & EXECUTE
        switch (opcode & 0xF000) {
            case 0x0000:
                if ((opcode & 0x00FF) == 0xE0) memset(myChip8.gfx, 0, sizeof(myChip8.gfx));
                else if ((opcode & 0x00FF) == 0xEE) { myChip8.sp--; myChip8.pc = myChip8.stack[myChip8.sp]; }
                break;
            case 0x1000: myChip8.pc = opcode & 0x0FFF; break;
            case 0x2000: myChip8.stack[myChip8.sp++] = myChip8.pc; myChip8.pc = opcode & 0x0FFF; break;
            case 0x7000: myChip8.V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF); break;
            case 0x8000: {
                uint8_t x = (opcode & 0x0F00) >> 8;
                uint8_t y = (opcode & 0x00F0) >> 4;
                switch (opcode & 0x000F) {
                    case 0x0: myChip8.V[x] = myChip8.V[y]; break;
                    case 0x4: myChip8.V[0xF] = (myChip8.V[x] + myChip8.V[y] > 255); myChip8.V[x] += myChip8.V[y]; break;
                }
            } break;
            case 0xA000: myChip8.I = opcode & 0x0FFF; break;
        }
        
        // --- Render ---
        for (int i = 0; i < 64 * 32; i++) pixels[i] = myChip8.gfx[i] ? 0xFFFFFFFF : 0xFF000000;
        SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_Delay(2); // Throttles speed to prevent CPU overload
    }

    SDL_DestroyTexture(texture); SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window); SDL_Quit();
    return 0;
}