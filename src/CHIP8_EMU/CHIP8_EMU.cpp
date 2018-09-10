// CHIP8_EMU.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <SDL/include/SDL.h>

#include "chip8.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

int bootSDL(SDL_Window* & window, SDL_Renderer* & renderer, SDL_Surface* & screenSurface) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Could not init SDL2: %s\n", SDL_GetError());
        return 1;
    }
    window = SDL_CreateWindow(
        "hello_SDL",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
	renderer = SDL_CreateRenderer(window, 0, 0);
    if (!window) {
        fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
        return 2;
    }
    screenSurface = SDL_GetWindowSurface(window);
    return 0;
}

void setupGraphics(SDL_Renderer* & renderer) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
}
void setupInput() {
    // TODO
}

void evaluateSDLinput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			//TODO
			break;
		}
	}
}

void drawGraphics(SDL_Renderer* & renderer, unsigned char gfx[], int width_res, int height_res) {
	// Res = 64 * 32
	for (int y = 0; y < height_res; ++y) {
		for (int x = 0; x < width_res; ++x) {
			if (gfx[y * height_res + x] == 1)
				SDL_RenderDrawPoint(renderer, x, y);
		}
	}
}

int main(int argc, char *args[])
{
    //http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
    SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
    SDL_Surface* screenSurface = nullptr;
    
    bootSDL(window, renderer, screenSurface);
	setupGraphics(renderer);

    Chip8 myChip8;
	myChip8.initialise();
	myChip8.loadGame("C:\\Users\\Braedyn\\Documents\\GitHub\\CHIP-8-Emulator\\roms\\programs\\Jumping X and O [Harry Kleinberg, 1977].ch8");
    
	for (;;) {
        myChip8.emulateCycle();

        // If the draw flag is set, update the screen
		if (myChip8.drawFlag) {
			drawGraphics(renderer, myChip8.graphics, 64, 32);
			SDL_RenderPresent(renderer);
		}
        
		// Store key press state (Press and Release)
        myChip8.setKeys();
    }

    SDL_Delay(2000);
	SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

