// CHIP8_EMU.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <SDL/include/SDL.h>

#include "chip8.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

int bootSDL(SDL_Window* & window, SDL_Surface* & screenSurface) {
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
    if (!window) {
        fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
        return 2;
    }
    screenSurface = SDL_GetWindowSurface(window);
    return 0;
}

void setupGraphics() {
    // TODO
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

int main(int argc, char *args[])
{
    //http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
    SDL_Window* window = nullptr;
    SDL_Surface* screenSurface = nullptr;
    
    bootSDL(window, screenSurface);

    Chip8 myChip8;
    for (;;) {
        myChip8.emulateCycle();

        // If the draw flag is set, update the screen
        if (myChip8.drawFlag)
            drawGraphics();

        // Store key press state (Press and Release)

        myChip8.setKeys();
    }

    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
    SDL_UpdateWindowSurface(window);
    SDL_Delay(2000);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

