#include "raylib.h"
#include "chip8.h"

int main() {
    chip8 cpu;
    chip8_init(&cpu);
    chip8_load_rom(&cpu, "roms/pong.ch8");

    InitWindow(640, 320, "CHIP-8 Emulator - Pong");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Map keys
        cpu.key[0x1] = IsKeyDown(KEY_ONE);
        cpu.key[0x2] = IsKeyDown(KEY_TWO);
        cpu.key[0x3] = IsKeyDown(KEY_THREE);
        cpu.key[0xC] = IsKeyDown(KEY_FOUR);

        // Update timers
        if (cpu.delay_timer > 0) cpu.delay_timer--;
        if (cpu.sound_timer > 0) cpu.sound_timer--;

        // Run emulation cycles
        for (int i = 0; i < 10; i++) {
            chip8_emulate_cycle(&cpu);
        }

        // Render
        BeginDrawing();
        ClearBackground(BLACK);
        if (cpu.draw_flag) {
            for (int i = 0; i < 2048; i++) {
                if (cpu.gfx[i]) DrawRectangle((i % 64) * 10, (i / 64) * 10, 10, 10, WHITE);
            }
            cpu.draw_flag = false; 
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}