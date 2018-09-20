#pragma once
#include "stdafx.h"

#include <fstream>
#include <random>
#include <iterator>
#include <algorithm>
#include <vector>
#include <string>
#include <array>


class Chip8 {
public:
	unsigned short opcode;
	// System has 4K memory
	std::array<unsigned char, 4096> memory;

	// CPU Registers, 15 * 8bit, V0->VE + the 16th VF
	// 16th (VF) is the carry flag
	std::array<unsigned char, 16> registerV;

	unsigned short indexRegister;
	unsigned short programCounter;

	// Memory Map
	// 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
	// 0x050 - 0x0A0 - Used for the built in 4x5 pixel font set(0 - F)
	// 0x200 - 0xFFF - Program ROM and work RAM

	// Drawing is done in XOR, which sets VF register (used for collision detection
	// Screen Res is total of 64 x 32
	std::array<unsigned char, 64 * 32> graphics;

	bool drawFlag = false;

	// No interupts/hardware registers
	
	// Timer registers at 60Hz, when > 0, they count down to 0
	unsigned char delayTimer;
	unsigned char soundTimer; // Buzzer sounds when == 0

	// Stack, anytime jump or subroutine used, the pc needs to be stored in the stack
	// System has 16 levels of stack, current level stored in stackPointer
	std::array<unsigned short, 16> stack;
	unsigned short stackPointer;

	// Hex keypad used for input - 0x0 -> 0xF, current state of key here
	std::array<unsigned char, 16> prevKey;
	std::array<unsigned char, 16> key;
	unsigned char lastPressedKey;

	std::array<unsigned char, 80> chip8_fontset =
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
    void clearDisplay() {
        // Clear display
		graphics = {};
		drawFlag = false;
    }
    void clearStack() {
        // Clear stack
		stack = {};
    }
    void clearRegisters() {
        // Clear register V0-VF
		registerV = {};
    }
    void clearMemory() {
        // Clear memory
		memory = {};
    }
	void clearKeys() {
		prevKey = {};
		key = {};
        lastPressedKey = 0;
	}
    void initialise() {
        // Initialise registers and memory once
		programCounter = 0x200; // Program counter starts at 0x200
		// Resets
		opcode = 0;
		indexRegister = 0;
		stackPointer = 0;

        clearDisplay();
        clearStack();
        clearRegisters();
        clearMemory();
		clearKeys();

		// Load fontset
		for (int i = 0; i < 80; ++i) {
			memory[i] = chip8_fontset[i];
		}

		// Reset timers
        delayTimer = 0;
        soundTimer = 0;
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

	int randRange(int min, int max) {
		// https://stackoverflow.com/questions/7560114/random-number-c-in-some-range
		std::random_device rd; // obtain random number from hardware
		std::mt19937 engine{ rd() }; // seed the generator
		std::uniform_int_distribution<int> distr{ min, max };

		return distr(engine);
	}

	void setKeys(const Uint8* keyboardState) {
		// Takes in the array from SDL the Uint8
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
		// There's probably a way better way to do this
		prevKey = key;
		
		key[0x0] = keyboardState[SDL_SCANCODE_X];
		key[0x1] = keyboardState[SDL_SCANCODE_1];
		key[0x2] = keyboardState[SDL_SCANCODE_2];
		key[0x3] = keyboardState[SDL_SCANCODE_3];
		key[0x4] = keyboardState[SDL_SCANCODE_Q];
		key[0x5] = keyboardState[SDL_SCANCODE_W];
		key[0x6] = keyboardState[SDL_SCANCODE_E];
		key[0x7] = keyboardState[SDL_SCANCODE_A];
		key[0x8] = keyboardState[SDL_SCANCODE_S];
		key[0x9] = keyboardState[SDL_SCANCODE_D];
		key[0xA] = keyboardState[SDL_SCANCODE_Z];
		key[0xB] = keyboardState[SDL_SCANCODE_C];
		key[0xC] = keyboardState[SDL_SCANCODE_4];
		key[0xD] = keyboardState[SDL_SCANCODE_R];
		key[0xE] = keyboardState[SDL_SCANCODE_F];
		key[0xF] = keyboardState[SDL_SCANCODE_V];

		for (int i = 0; i < 16; ++i) {
			if (key[i] != prevKey[i]) {
				lastPressedKey = i;
				break;
			}
		}
	}


    void emulateCycle() {
        // Fetch opcode
		// Each one is 2 bytes that has to be combined
		opcode = memory[programCounter] << 8 | memory[programCounter + 1];

		// Decode opcode
		switch (opcode & 0xF000) {
			// Counter incremented by 2, as two successive bytes are fetched
			// from different addresses, and merged

		case 0x0000: {
            switch (opcode & 0x0FFF) {
            case 0x00E0: { // 0x00E0: Clears the screen
                clearDisplay();
                programCounter += 2;
                break;
            }
            case 0x00EE: { // 0x00EE: Returns from subroutine
                --stackPointer;
                programCounter = stack[stackPointer];
				programCounter += 2;
                // Might need to ++2 the pc ?
                break;
            }
            default: { // 0xNNN: Calls RCA 1802 program at address NNN. Not necessary for most ROMs.
                // TODO
                break;
            }
            }
			break;
		}

		
        case 0x1000: { // 0x1NNN: Jumps to address NNN
            programCounter = opcode & 0x0FFF;
            // += 2 not necessary
            break;
        }
        case 0x2000: { // 0x2NNN: Calls subroutine at address NNN
                       // program counter NOT increased by two, because subroutine
            stack[stackPointer] = programCounter;
            ++stackPointer;
            programCounter = opcode & 0x0FFF;
            break;
        }
        case 0x3000: { // 0x3XNN: 
                       // Skips the next instruction if VX equals NN. 
                       // (Usually the next instruction is a jump to skip a code block)
            int x = (opcode & 0x0F00) >> 8;
            int nn = opcode & 0x00FF;
            programCounter += 2;
            if (registerV[x] == nn) {
                programCounter += 2;
            }

            break;
        }
		case 0x4000: { // 0x4XNN: Skips the next instruction if VX doesn't equal NN.
			programCounter += 2;
            int x = (opcode & 0x0F00) >> 8;
            int nn = opcode & 0x00FF;
            if (registerV[x] != nn) {
                programCounter += 2;
            }
			break;
		}
		case 0x5000: { // 0x5XY0: Skips the next instruction if VX equals VY
			programCounter += 2;
			if (registerV[(opcode & 0x0F00) >> 8] == registerV[(opcode & 0x00F0) >> 4])
				programCounter += 2;
			break;
		}
        case 0x6000: { // 0x6XNN: Sets VX to NN
            registerV[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            programCounter += 2;
            break;
        }
        case 0x7000: { // 0x7XNN: Adds NN to VX (Carry flag not changed)
            registerV[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            programCounter += 2;
            break;
        }
        case 0x8000: {
            switch (opcode & 0x000F) {
			case 0x0000: { // 0x8XY0: Sets VX to VY
				registerV[(opcode & 0x0F00) >> 8] = registerV[(opcode & 0x00F0) >> 4];
				programCounter += 2;
				break;
			}
			case 0x0001: { // 0x8XY1: Sets VX to VX or VY. (Bitwise OR operation)
				registerV[(opcode & 0x0F00) >> 8] = 
					  registerV[(opcode & 0x0F00) >> 8]
					| registerV[(opcode & 0x00F0) >> 4];
				programCounter += 2;
				break;
			}
			case 0x0002: { // 0x8XY2: Sets VX to VX and VY. (Bitwise AND operation)
				registerV[(opcode & 0x0F00) >> 8] =
					  registerV[(opcode & 0x0F00) >> 8]
					& registerV[(opcode & 0x00F0) >> 4];
				programCounter += 2;
				break;
			}
			case 0x0003: { // 0x8XY3: Sets VX to VX xor VY. (Bitwise XOR operation)
				registerV[(opcode & 0x0F00) >> 8] =
					  registerV[(opcode & 0x0F00) >> 8]
					^ registerV[(opcode & 0x00F0) >> 4];
				programCounter += 2;
				break;
			}
            case 0x0004: { // 0x8XY4: adds value of VY to VX
                // If sum is greater than 255, carry flag lets us know
                if (registerV[(opcode & 0x00F0) >> 4] > (0xFF - registerV[(opcode & 0x0F00) >> 8]))
                    setCarry(true);
                else
                    setCarry(false);
                registerV[(opcode & 0x0F00) >> 8] += registerV[(opcode & 0x00F0) >> 4];
                programCounter += 2;
                break;
            }
			case 0x0005: { // 0x8XY5: VY is subtracted from VX
				// Carry Flag set to 0 if borrow, 1 if isn't
				int x = (opcode & 0x0F00) >> 8;
				int y = (opcode & 0x00F0) >> 4;
				if (registerV[x] > registerV[y])
					setCarry(true);
				else
					setCarry(false);

				registerV[x] -= registerV[y];
				programCounter += 2;
			}
			case 0x0006: { // 0x8XY6: Stores least significant bit of VX in VF
				// Also shifts VX to right by 1
				// Docs disagree on what this is, may have to divide VX by two instead of shift
				registerV[0xF] = (registerV[(opcode & 0x0F00) >> 8] & 0b00000001);
				registerV[(opcode & 0x0F00) >> 8] >>= 1;
				programCounter += 2;
				break;
			}
			case 0x0007: { // 0x8XY7: Sets VX to VY minus VX
				// VF is set to 0 when there's a borrow, and 1 when there isn't.
				int x = (opcode & 0x0F00) >> 8;
				int y = (opcode & 0x00F0) >> 4;
				if (registerV[x] > registerV[y])
					setCarry(true);
				else
					setCarry(false);
				registerV[x] = registerV[y] - registerV[x];
				programCounter += 2;
				break;
			}
			case 0x000E: { // 0x8XYE: Stores the most significant bit of VX in VF 
				// and then shifts VX to the left by 1
				registerV[0xF] = (registerV[(opcode & 0x0F00) >> 8] & 0b10000000) >> 7;
				registerV[(opcode & 0x0F00) >> 8] <<= 1;
				programCounter += 2;
				break;
			}
            default:
                printf("UNKNOWN OPCODE: 0x%X\n", opcode);
                programCounter += 2;
                break;
            }
            break;
        }
		case 0x9000: { // 0x9XY0: Skips next instructions if VX != VY
			programCounter += 2;
			if (registerV[(opcode & 0x0F00) >> 8] != registerV[(opcode & 0x00F0) >> 4])
				programCounter += 2;
			break;
		}
		
        case 0xA000: { // 0xANNN: Sets indexRegister to address NNN
            indexRegister = opcode & 0x0FFF;
            programCounter += 2;
            break;
        }
		case 0xB000: { // 0xBNNN: Jumps to address NNN plus V0
			stack[stackPointer] = programCounter;
			programCounter = (opcode & 0x0FFF) + registerV[0];
			break;
		}
		case 0xC000: { // 0xCXNN: Sets VX to bitwise AND on a random number and NN
			// random in range 0-255
			registerV[(opcode & 0x0F00) >> 8] = randRange(0, 255) & (opcode & 0x00FF);
			programCounter += 2;
			break;
		}
		
		case 0xD000: { // 0xDXYN: Draws sprite at VX,VY with width of 8 and height of N
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
                        if ((x + xline + ((y + yline) * 64)) > 2047)
                            continue; // Some graphics go off screen - which breaks the limits of the array, and crash
						if (graphics[(x + xline + ((y + yline) * 64))] == 1)
							setCarry(true);
						graphics[x + xline + ((y + yline) * 64)] ^= 1;
					}
				}
			}

			drawFlag = true;
			programCounter += 2;
			break;
		}

		case 0xE000: // Multiple options here
		{
			// 0xEX9E, 0xEXA1, and 0xFX0A
			switch (opcode & 0x00FF) {
			case 0x009E: { // 0xEX9E: Skips next instruction if key in VX pressed
				if (key[registerV[(opcode & 0x0F00) >> 8]] != 0)
					programCounter += 2;
				programCounter += 2;
				break;
			}
			case 0x00A1: { // 0xEXA1: Skips next instruction if key in VX not pressed
				if (key[registerV[(opcode & 0x0F00) >> 8]] == 0)
					programCounter += 2;
				programCounter += 2;
				break;
			}
			default:
				printf("UNKNOWN OPCODE: 0x%X\n", opcode);
				programCounter += 2;
				break;
			}
			break;
		}
		case 0xF000: // Multiple options
		{
			switch (opcode & 0x00FF) {
			case 0x0007: { // 0xFX07: Sets VX to the value of the delay timer.
				int x = (opcode & 0x0F00) >> 8;
				registerV[x] = delayTimer;
				programCounter += 2;
				break;
			}
			case 0x000A: { // 0xFX0A: Key press waited for, then stored in VX
				if (prevKey != key) { // TODO;
					registerV[(opcode & 0x0F00) >> 8] = lastPressedKey;
					programCounter += 2; // Does not continue unless key pressed
				}
				break;
			}
			case 0x0015: { // 0xFX15: Set delay timer to VX
				int x = (opcode & 0x0F00) >> 8;
				delayTimer = registerV[x];
				programCounter += 2;
				break;
			}
			case 0x0018: { // 0xFX18: Sets sound timer to VX
				soundTimer = registerV[(opcode & 0x0F00) >> 8];
				programCounter += 2;
				break;
			}
			case 0x001E: { // 0xFX1E: Adds VX to I
				if (indexRegister + registerV[(opcode & 0x0F00) >> 8] > 0xFFF)
					setCarry(true);
				else
					setCarry(false);
				indexRegister += registerV[(opcode & 0x0F00) >> 8];
				programCounter += 2;
				break;
			}
			case 0x0029: { // 0xFX29: Sets I to location of the sprite for char in VX
				// Need to evaluate
				unsigned char vxChar = registerV[(opcode & 0x0F00) >> 8];
				static const int fontWidth = 5;
				//indexRegister = chip8_fontset[fontWidth * vxChar];
				indexRegister = fontWidth * vxChar;
				programCounter += 2;
				break;
			}
            case 0x0033: { // 0xFX33: Store binary-coded decimal representation of VX at I, I+1 and I+2
                memory[indexRegister] = registerV[(opcode & 0x0F00) >> 8] / 100;
                memory[indexRegister + 1] = (registerV[(opcode & 0x0F00) >> 8] / 10) % 10;
                memory[indexRegister + 2] = (registerV[(opcode & 0x0F00) >> 8] % 100) % 10;
                programCounter += 2;
                break;
            }
			case 0x0055: { // 0xFX55: Stores v0 to vX in memory at address I
				// (Including Vx)
				// Offset from I is increased by 1 for each value, but I not modified
				int x = (opcode & 0x0F00) >> 8;
				for (int i = 0; i <= x; ++i) {
					memory[indexRegister + i] = registerV[i];
				}
				programCounter += 2;
				break;
			}
			case 0x0065: { // 0xFX65: Fills v0 to Vx with values from memory starting at I
				// (Including Vx)
				int x = (opcode & 0x0F00) >> 8;
				for (int i = 0; i <= x; ++i) {
					registerV[i] = memory[indexRegister + i];
				}
				programCounter += 2;
				break;
			}
			default:
				printf("UNKNOWN OPCODE: 0x%X\n", opcode);
				programCounter += 2;
                break;
			}

			break;
		}
		default:
			printf("UNKNOWN OPCODE: 0x%X\n", opcode);
			programCounter += 2;
            break;
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