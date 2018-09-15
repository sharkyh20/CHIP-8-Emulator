// CHIP8_EMU.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <string>
#include <iostream>
#include <vector>


#include <SDL/include/SDL.h>

#include <nfd.h>

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
void clearScreen(SDL_Renderer* & renderer) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
}
void setupGraphics(SDL_Renderer* & renderer) {
	clearScreen(renderer);
    SDL_RenderSetScale(renderer, 5, 5);
}
void setupInput() {
    // TODO
}

bool getUserFileChoice(nfdchar_t* &chosenFilePath) {
    // Load File Dialogue
    nfdchar_t *outPath = NULL;
    nfdresult_t result = NFD_OpenDialog(NULL, NULL, &outPath);

    if (result == NFD_OKAY) {
        puts("Success!");
        puts(outPath);
        chosenFilePath = outPath;
        return true;
    }
    else if (result == NFD_CANCEL) {
        puts("User pressed cancel.");
    }
    else {
        printf("Error: %s\n", NFD_GetError());
    }
    return false;
}

bool fileIsChip8(std::string chosenFilePath) {
    // Credit to '17 of 26'
    // https://stackoverflow.com/questions/51949/how-to-get-file-extension-from-string-in-c
    std::string::size_type idx;
    idx = chosenFilePath.rfind('.');

    if (idx != std::string::npos)
    {
        std::string extension = chosenFilePath.substr(idx + 1);
        if (extension == "ch8")
            return true;
        else
            return false;
    }
    else
    {
        // No extension found at all - should not happen, only file paths should be passed
        printf("ERROR: Folder path passed to fileIsChip8, expected File path!");
        throw;
    }
}

const Uint8* evaluateSDLinput() {
	SDL_Event event;
	SDL_PumpEvents();
	return SDL_GetKeyboardState(NULL);
}

void drawGraphics(SDL_Renderer* & renderer, std::array<unsigned char, 2048> gfx, int width_res, int height_res) {
	// Res = 64 * 32
	for (int y = 0; y < height_res; ++y) {
		for (int x = 0; x < width_res; ++x) {
            if (gfx[y * width_res + x] == 1) {
                SDL_RenderDrawPoint(renderer, x, y);
            }
		}
	}
}
#define TICK_INTERVAL    60

Uint32 time_left(Uint32 nextTime)
{
    Uint32 now;

    now = SDL_GetTicks();
    if (nextTime <= now)
        return 0;
    else
        return nextTime - now;
}


int main(int argc, char *argv[])
{
    //http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
    SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
    SDL_Surface* screenSurface = nullptr;
    
    bootSDL(window, renderer, screenSurface);
	setupGraphics(renderer);

    Chip8 myChip8;
	myChip8.initialise();


    nfdchar_t* filePath = nullptr;
    bool fileSelected = getUserFileChoice(filePath);
    if (fileSelected && fileIsChip8(filePath)) {
        myChip8.loadGame(filePath);
    }
    else {
        // For now, if user cancels, program just closes to prevent undefined behaviour
        printf("NO FILE SELECTED, ABORTING");
        SDL_Delay(2000);
        return 0;
    }
	
    int opcodesPerSecond = 600;
    Uint32 nextTime = SDL_GetTicks() + TICK_INTERVAL;
	for (;;) {
        long long seconds = 0;
        for (int i = 0; i < opcodesPerSecond / TICK_INTERVAL; ++i) { // one frame
            // Store key press state (Press and Release)
            myChip8.setKeys(evaluateSDLinput());

            myChip8.emulateCycle();

            // If the draw flag is set, update the screen
            if (myChip8.drawFlag) {
                clearScreen(renderer);
                drawGraphics(renderer, myChip8.graphics, 64, 32);
                SDL_RenderPresent(renderer);
            }
        }
        SDL_Delay(time_left(nextTime));
        nextTime += TICK_INTERVAL;
    }

    SDL_Delay(2000);
	SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

