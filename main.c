#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <SDL.h>
#include <err.h>

#define byte uint8_t
#define MEM_SIZE 0x1000000
#define MEM_PADDED_SIZE (MEM_SIZE + 8)
#define SCREEN_X 256
#define SCREEN_Y 256
#define SCREEN_SCALE 4

typedef struct color { byte a; byte r; byte g; byte b; } color_t;

color_t screen[SCREEN_Y][SCREEN_X];
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* buffer = NULL;

byte mem[MEM_PADDED_SIZE];
int pc = 0;

void load_rom() {
    FILE* fp = fopen("nyan.bp", "rb");

    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    byte* buf = malloc(size);
    fread(buf, size, 1, fp);
    printf("Loaded %zu bytes\n", size);

    memcpy(mem, buf, size);

    free(buf);
}

void render_frame() {
    pc = mem[2] << 16 | mem[3] << 8 | mem[4];
    for (int i = 0; i < 65536; i++) {
        int A = mem[pc]     << 16 | mem[pc + 1] << 8 | mem[pc + 2];
        int B = mem[pc + 3] << 16 | mem[pc + 4] << 8 | mem[pc + 5];

        mem[B] = mem[A];

        pc = mem[pc + 6] << 16 | mem[pc + 7] << 8 | mem[pc + 8];
    }

    for (int x = 0; x < SCREEN_X; x++) {
        for (int y = 0; y < SCREEN_Y; y++) {
            byte pixel = mem[(mem[5] << 16) | (y << 8) | x];
            screen[y][x].b = (pixel % 6) * 0x33;
            pixel /= 6;
            screen[y][x].g = pixel % 6 * 0x33;
            pixel /= 6;
            screen[y][x].r = pixel % 6 * 0x33;
            screen[y][x].a = 0xFF;
        }
    }

    SDL_UpdateTexture(buffer, NULL, screen, SCREEN_X * 4);
    SDL_RenderCopy(renderer, buffer, NULL, NULL);
    SDL_RenderPresent(renderer);

}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        errx(EXIT_FAILURE, "SDL couldn't initialize! %s", SDL_GetError());
    }
    window = SDL_CreateWindow("dgb bytepusher", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_X * SCREEN_SCALE, SCREEN_Y * SCREEN_SCALE, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_STREAMING, SCREEN_X, SCREEN_Y);

    bool should_render_frame = true;
    memset(mem, 0, MEM_PADDED_SIZE);
    load_rom();
    while (should_render_frame) {
        render_frame();
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    should_render_frame = false;
                    break;
            }
        }
    }

    return 0;
}
