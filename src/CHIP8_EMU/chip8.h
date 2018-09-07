#pragma once
#include "stdafx.h"

#include <fstream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <string>


class Chip8 {
public:
	unsigned short opcode;
	// System has 4K memory
	unsigned char memory[4096];

	// CPU Registers, 15 * 8bit, V0->VE + the 16th VF
	// 16th (VF) is the carry flag
	unsigned char registerV[16];

	unsigned short indexRegister;
	unsigned short programCounter;

	// Memory Map
	// 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
	// 0x050 - 0x0A0 - Used for the built in 4x5 pixel font set(0 - F)
	// 0x200 - 0xFFF - Program ROM and work RAM

	// Drawing is done in XOR, which sets VF register (used for collision detection
	// Screen Res is total of 64 x 32
	unsigned char graphics[64 * 32];

	bool drawFlag = false;

	// No interupts/hardware registers
	
	// Timer registers at 60Hz, when > 0, they count down to 0
	unsigned char delayTimer;
	unsigned char soundTimer; // Buzzer sounds when == 0

	// Stack, anytime jump or subroutine used, the pc needs to be stored in the stack
	// System has 16 levels of stack, current level stored in stackPointer
	unsigned short stack[16];
	unsigned short stackPointer;

	// Hex keypad used for input - 0x0 -> 0xF, current state of key here
	unsigned char key[16];

	unsigned char chip8_fontset[80] =
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};
    void initialise() {
        // Initialise registers and memory once
		programCounter = 0x200; // Program counter starts at 0x200
		// Resets
		opcode = 0;
		indexRegister = 0;
		stackPointer = 0;

		// Clear display
		// Clear stack
		// Clear register V0-VF
		// Clear memory

		// Load fontset
		for (int i = 0; i < 80; ++i) {
			memory[i] = chip8_fontset[i];
		}

		// Reset timers
    }
    void loadGame(std::string gameName) {
        // Load file in binary mode, fill at 0x200 ( = 512)
		std::ifstream file(gameName, std::ios::binary);
		std::vector<char> buffer((
			std::istreambuf_iterator<char>(file)),
			(std::istreambuf_iterator<char>()));
		for (int i = 0; i < buffer.size(); ++i) {
			memory[i + 0x200] = buffer[i];
		}
    }

	void setCarry(bool on) { registerV[0xF] = on; }

	void setKeys() {
		/*
		Keypad                   Keyboard
		+-+-+-+-+                +-+-+-+-+
		|1|2|3|C|                |1|2|3|4|
		+-+-+-+-+                +-+-+-+-+
		|4|5|6|D|                |Q|W|E|R|
		+-+-+-+-+       =>       +-+-+-+-+
		|7|8|9|E|                |A|S|D|F|
		+-+-+-+-+                +-+-+-+-+
		|A|0|B|F|                |Z|X|C|V|
		+-+-+-+-+                +-+-+-+-+
		*/
		
	}

    void emulateCycle() {
        // Fetch opcode
		// Each one is 2 bytes that has to be combined
		opcode = memory[programCounter] << 8 | memory[programCounter + 1];

		// Decode opcode
		switch (opcode & 0xF000) {
			// Counter incremented by 2, as two successive bytes are fetched
			// from different addresses, and merged

		case 0x0000: // 0x00E0: Clears the screen
			// Execute
			break;

		case 0x000E: // 0x00EE: Returns from subroutine
			// Execute
			break;

		case 0xA000: // 0xANNN: Sets indexRegister to address NNN
			indexRegister = opcode & 0x0FFF;
			programCounter += 2; 
			break;

		case 0x2000: // 0x2NNN: Calls subroutine at address NNN
			// program counter NOT increased by two, because subroutine
			stack[stackPointer] = programCounter;
			++stackPointer;
			programCounter = opcode & 0x0FFF;
			break;

		case 0x0004: // 0x8XY4: adds value of VY to VX
			// If sum is greater than 255, carry flag lets us know
			if (registerV[(opcode & 0x00F0) >> 4] > (0xFF - registerV[(opcode & 0x0F00) >> 8]))
				setCarry(true);
			else
				setCarry(false);
			registerV[(opcode & 0x0F00) >> 8] += registerV[(opcode & 0x00F0) >> 4];
			programCounter += 2;
			break;

		case 0x0033: // 0xFX33: Store binary-coded decimal representation of VX at I, I+1 and I+2
			memory[indexRegister] = registerV[(opcode & 0x0F00) >> 8] / 100;
			memory[indexRegister + 1] = (registerV[(opcode & 0x0F00) >> 8] / 10) % 10;
			memory[indexRegister + 2] = (registerV[(opcode & 0x0F00) >> 8] % 100) % 10;
			programCounter += 2;
			break;

		case 0xD000: // 0xDXYN: Draws sprite at VX,VY with width of 8 and height of N
			// Carry set if screen pixels are flipped from set to unset when drawn
			// Carry stays false if this doesn't happen
			unsigned short x = registerV[(opcode & 0x0F00) >> 8];
			unsigned short y = registerV[(opcode & 0x00F0) >> 4];
			unsigned short height = opcode & 0x000F;
			unsigned short pixel;

			setCarry(false);
			for (int yline = 0; yline < height; yline++) {
				pixel = memory[indexRegister + yline];
				for (int xline = 0; xline < 8; xline++) {
					if ((pixel & (0x80 >> xline)) != 0) {
						if (graphics[(x + xline + ((y + yline) * 64))] == 1)
							setCarry(true);
						graphics[x + xline + ((y + yline) * 64)] ^= 1;
					}
				}
			}

			drawFlag = true;
			programCounter += 2;
			break;
			
		case 0xE000: // Multiple options here
			// 0xEX9E, 0xEXA1, and 0xFX0A
			switch (opcode & 0x00FF) {
			case 0x009E: // EX9E: Skips next instruction if key in VX pressed
				if (key[registerV[(opcode & 0x0F00) >> 8]] != 0)
					programCounter += 4;
				else
					programCounter += 2;
				break;
			}
			break;
		default:
			printf("UNKNOWN OPCODE: 0x%X\n", opcode);
		}

        // Update timers
		if (delayTimer > 0)
			--delayTimer;

		if (soundTimer > 0) {
			if (soundTimer == 1)
				printf("BEEP!\n");
			--soundTimer;
		}
    }
};