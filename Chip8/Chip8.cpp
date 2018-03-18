#include "stdio.h"  // debug and loader only
#include "stdlib.h"
#include "Chip8.h" 

chip8::chip8()
{
	// empty constructor
}

chip8::~chip8()
{
	// empty destructor
}

void chip8::initialize()
{
	// init registers and memory once
	pc = 0x200;    // program counter always starts at 0x200
	opcode = 0;        // reset current opcode
	I = 0;        // reset index register
	sp = 0;        // reset stack pointer

				   // clear display
	for (int i = 0; i < (64 * 32); ++i)
	{
		gfx[i] = 0;
	}

	// clear stack
	for (int i = 0; i < STACK_SIZE; ++i)
	{
		stack[i] = 0;
	}

	// clear registers V0 through VF
	for (int i = 0; i < 16; ++i)
	{
		V[i] = 0;
	}

	// clear keyset
	for (int i = 0; i < 16; ++i)
	{
		key[i] = 0;
	}

	// clear memory
	for (int i = 0; i < MEMORY_SIZE; ++i)
	{
		memory[i] = 0;
	}

	// load fontset
	for (int i = 0; i < 80; ++i)
	{
		memory[i] = chip8_fontset[i];
	}

	// reset timers
	delay_timer = 0;
	sound_timer = 0;

	// signal a screen clear
	drawFlag = true;

	//srang (time(NULL)); here, not sure what for....
}

void chip8::emulateCycle()
{
	// FETCH OPCODE
	// each opcode is 2 bytes long, need to get pc and pc+1 to get the full
	//  opcode then merge them with bitwise OR operator
	//  Memory:  
	//         0xA2 0xF0   0x34 0x22
	//         ---------   ---------
	//         opcode      next opcode
	//  
	//  opcode = 0x00 0xA2
	//  opcode = 0xA2 0x00  (0xA2) << 8
	//  opcode = 0xA2 0xF0  
	opcode = memory[pc] << 8 | memory[pc + 1];

	// DECODE OPCODE
	// look at the first 4 bits to determine most opcodes
	switch (opcode & 0xF000)
	{
		// execute opcodes

	// set the index address 'I'
	case 0xA000: {              // ANNN:  Sets I to the address NNN
		I = opcode & 0x0FFF;    // - set I to the NNN part of the opcode
		pc += 2;                // - move the program counter by 2 for next opcode
		break;
	}
		// TODO:  more opcodes
		// try getting a function pointer map going where each opcode is 
		//   mapped to its specific execute function
		// decoding may still need to be done via the switch statement here to get 
		//   the proper key for the map and then call the execute function with the 
		//   rest of the opcode data.
		//
		//  Example flow:
		//        1. Decode opcode with switch
		//        2. Get execute function based on decode result
		//          - note: there should be 255 execute functions for each opcode that exists?
		//        3. Call the execute function with the remaining data from the opcode

	// jump to function call
	case 0x2000: {
		stack[sp] = pc;    // store the current pc in the stack
		++sp;			   // increase the stack pointer to next avail location
		pc = opcode + 0x0FFF;	// set the pc to the address specified in the opcode (NNN part of 0x2NNN)
		break;
	}

	// draw a pixel on screen
	case 0xD000: {
		unsigned short x = V[(opcode & 0x0F00) >> 8];
		unsigned short y = V[(opcode & 0x00F0) >> 4];
		unsigned short height = opcode & 0x000F;
		unsigned short pixel;

		// reset register VF (this is the drawFlag and pixel collision register - status register)
		V[0xF] = 0;

		// loop through the height of the pixel
		for (int yline = 0; yline < height; ++yline) {
			
			// get the pixel to be drawn from memory - pixel will be at 'I', but need to get the full height of it via 'height'
			pixel = memory[I + yline];

			// scan through the bits of the pixel obtained from memory (8 bits - use 0x10000000, or 0x80, and shift right to check)
			for (int xline = 0; xline < 8; ++xline) {

				// check if the bit is set to '1'
				if (pixel & (0x80 >> xline) != 0) {

					// check the location to be drawn on screen for any current pixel being displayed
					// current pixel on screen is at the location specified by x and y in the V registers and accounting for the height of the pixel
					if (gfx[(x + xline) + ((y + yline) * 64)] == 1) {
						// if the pixel bit is '1' and the same gfx bit is '1', we have a pixel collision, set the VF register
						V[0xF] = 1;
					}
					// now XOR '1' with the gfx register to get the final pixel value to draw (on or off)
					gfx[(x + xline) + ((y + yline) * 64)] ^= 1;
				}
			}
		}
	}

	default:
		debug_fmt_msg("Uknown opcode: 0x%X\n", opcode);
	}

	// update timers
	if (delay_timer > 0)
	{
		--delay_timer;
	}

	if (sound_timer > 0)
	{
		if (sound_timer == 1)
		{
			debug_simple_msg("BEEP!\n");
			--sound_timer;
		}
	}
}

bool chip8::loadGame(char *filename)
{
	// initialize chip8
	initialize();

	debug_fmt_msg("Loading filename: %s\n", filename);

	// open file for reaching in binary mode (do not map end of line)
	FILE *ptrFile = NULL;
	fopen_s(&ptrFile, filename, "rb");
	if (ptrFile == NULL)
	{
		fputs("File does not exist!", stderr);
		return false;
	}

	// get the buffer size (size of file)
	fseek(ptrFile, 0, SEEK_END);
	long bufferSize = ftell(ptrFile);
	rewind(ptrFile);
	debug_fmt_msg("Filesize found to be: %d\n", bufferSize);

	// buffer should have 'bufferSize' number of bytes for the system
	char *buffer = (char*)malloc(sizeof(char) * bufferSize);
	if (buffer == NULL)
	{
		fputs("Memory allocation error", stderr);
	}

	// program or game is loaded into memory starting at location 0x200 (512 in decimal)
	for (int i = 0; i < bufferSize; ++i)
	{
		memory[i + 512] = buffer[i];
	}
	return true;
}

void chip8::setKeys() 
{
	// do nothing...
}

void chip8::debug_simple_msg(char *message)
{
	printf(message);
}

template <typename T>
void chip8::debug_fmt_msg(char *formatted_message, T object)
{
	printf(formatted_message, object);
}

