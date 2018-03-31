#include "stdio.h"  // debug and loader only
#include "stdlib.h"
#include "Chip8.h" 
#include "Debug.h"

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

	// opcode 0x0NNN -> Calls RCA 1802 program at address NNN. Not necessary for most ROMs.
	// opcode 0x00E0 -> Clears the screen
	// opcode 0x00EE -> Returns from a subroutine
	case 0x0000: 
		switch (opcode & 0x0FFF) {
			case 0x00E0: 
				// clear the screen
				for (int i = 0; i < (64 * 32); ++i) {
					gfx[i] = 0x0;
				}
				drawFlag = true;
				pc += 2;
				break;
			
			case 0x00EE: 
				// pop the stack and return to where the pc pointer was
				--sp;
				pc = stack[sp];  // return to the point of function call
				pc += 2;    // increase the pc to the next instruction after the function call
				break;
			

			// opcode 0x0NNN case -> this is probably not necessary
			default: 
				stack[sp] = pc;  // store the current pc on the stack
				++sp;			 // increment stack pointer
				pc = (opcode & 0x0FFF);  // set the pc to the address specified in the opcode (NNN part of 0x0NNN)
				break;
		}
		break;

	// opcodes 0x1NNN -> jump to address specified in 'NNN' 
	case 0x1000: 
		pc = (opcode & 0x0FFF);
		break;

	// opcodes 0x2NNN -> call subroutine (subroutine will return)
	case 0x2000: 
		stack[sp] = pc;    // store the current pc in the stack
		++sp;			   // increase the stack pointer to next avail location
		pc = opcode & 0x0FFF;	// set the pc to the address specified in the opcode (NNN part of 0x2NNN)
		break;

	// opcode 0x3XNN -> Skip the next instruction if VX equals NN
	case 0x3000: 
		if ((V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))) {
			pc += 4;   // skip the next instruction 
		}
		else {
			pc += 2;   // otherwise, just go to the next instruction
		}
		break;	

	// opcode 0x4XNN -> Skips the next instruction if VX doesn't equal NN.
	case 0x4000: 
		if ((V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))) {
			pc += 4;  // skip the next instruction
		}
		else {
			pc += 2;  // otherwise, just go to the next instruction
		}
		break;
		
	// opcode 0x5XY0 -> Skips the next instruction if VX equals VY. 
	case 0x5000:
		if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
			pc += 4;  // skip the next instruction
		}
		else {
			pc += 2;  // otherwise, just go to the next instruction
		}
		break;

	// opcode 0x6XNN -> Sets VX to NN.
	case 0x6000:
		V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
		pc += 2; 
		break;

	// opcode 0x7XNN -> Adds NN to VX. (Carry flag is not changed)
	case 0x7000:
		V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
		pc += 2;

	// opcodes 0x8XY? -> Last bit determines the operation
	case 0x0800:
		switch (opcode & 0x000F) {

		// opcode 0x8XY0 -> Sets VX to the value of VY
		case 0x0000:
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		// opcode 0x8XY1 -> Sets VX to VX or VY (Bitwise OR operation)
		case 0x0001:
			V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		// opcode 0x8XY2 -> Sets VX to VX and VY. (Bitwise AND operation)
		case 0x0002: 
			V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		// opcode 0x8XY3 -> Sets VX to VX xor VY.
		case 0x0003:
			V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		// opcode 0x8XY4 -> Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't
		case 0x0004:
			// we figure out how much of 0xFF is left when subtracting what is already in V[X]
			//   then if what is going to be added in V[Y] to that is more, then adding V[Y] to V[X] will cause overflow
			//  
			//   Example:
			//          Will 4 + 3 overflow 9?  ->  (9 - 4) = 5.   3 being added is not > 5, so no
			//          Will 4 + 7 overflow 9?  ->  (9 - 4) = 5.   7 being added is > 5, so yes
			if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])) {
				V[0xF] = 1;  // set carry flag
			}
			else {
				V[0xF] = 0; 
			}
			V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];    // now do the addition
			pc += 2;
			break;

		// opcode 0x8XY5 -> VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
		case 0x0005:
			// it is easier than above to figure out if we'll need to borrow
			//  we just need to determine if VY is bigger than VX - if it is, VX will go below 0 and need to borrow
			if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]) {
				V[0xF] = 1;  // set borrow flag
			}
			else {
				V[0xF] = 0; 
			}
			V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];    // now do the subtraction
			pc += 2;
			break;

		// opcode 0x8XY6 -> Shifts VY right by one and copies the result to VX. 
		//					VF is set to the value of the least significant bit of VY before the shift
		case 0x0006:
			V[0xF] = V[(opcode & 0x00F0) >> 4] & 0x01;
			V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x00F0) >> 4] >>= 1);
			pc += 2;
			break;

		// opcode 0x8XY7 -> Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
		case 0x0007:
			// if we're subtracting VX from VY, if VX is larger than VY, there will be a borrow
			if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]) {
				V[0xF] = 0;
			}
			else {
				V[0xF] = 1;
			}
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;

		// opcode 0x8XYE -> Shifts VY left by one and copies the result to VX. 
		//					VF is set to the value of the most significant bit of VY before the shift
		case 0x000E:
			V[0xF] = V[(opcode & 0x00F0) >> 4] & 0x80;   // msb = 0b10000000 = 0x80
			V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x00F0) >> 4] <<= 1);
			pc += 2;
			break;

		default:
			char msg[] = "Unknown opcode %X";
			debug_fmt_msg(msg, opcode);
			debug_simple_msg("Bad program - exiting!");
			exit(1);
		}

		
	// opcode 0x9XY0 -> Skips the next instruction if VX doesn't equal VY.
	case 0x9000:
		if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
			pc += 4;  // skip next instruction
		}
		else {
			pc += 2;
		}
		break;

	// opcode 0xANNN -> Sets I to the address NNN.
	case 0xA000:                // ANNN:  Sets I to the address NNN
		I = opcode & 0x0FFF;    // - set I to the NNN part of the opcode
		pc += 2;                // - move the program counter by 2 for next opcode
		break;

	// opcode 0xBNNN -> Jumps to the address NNN plus V0.
	case 0xB000:
		pc = V[0x0] + (opcode & 0x0FFF);
		break;

	// opcode 0xCXNN -> Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
	case 0xC000:
		V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) && (opcode & 0x00FF);
		pc += 2;
		break;

	// opcodes 0xDXYN -> draw a sprite on screen (sprite = 8 pixels wide, (opcode & 0x000F) pixels high)
	case 0xD000: 
		unsigned short x = V[(opcode & 0x0F00) >> 8];
		unsigned short y = V[(opcode & 0x00F0) >> 4];
		unsigned short height = opcode & 0x000F;
		unsigned short sprite_pixel;

		// reset register VF (this is the drawFlag and pixel collision register - status register)
		V[0xF] = 0;

		// loop through the height of the sprite
		for (int yline = 0; yline < height; ++yline) {
			
			// get the sprite to be drawn from memory - sprite will be at 'I', but need to scan through the entire height
			sprite_pixel = memory[I + yline];

			// scan through the bits of the sprite pixel obtained from memory (8 bits - use 0x10000000, or 0x80, and shift right to check)
			for (int xline = 0; xline < SPRITE_WIDTH; ++xline) {

				// check if the bit is set to '1' - if it's 0, we don't bother changing anything in memory now
				if (sprite_pixel & (0x80 >> xline) != 0) {

					// check the location to be drawn on screen for any current pixel being displayed
					// match a current screen (gfx[]) pixel by:
					//   1. start at x and y given by the V registers
					//   2. go through bit by bit across the sprite
					//   3. also go through the height of the pixel by using yline (height of to be drawn pixel) and moving 64 across (using a single dimensional array)
					if (gfx[(x + xline) + ((y + yline) * 64)] == 1) {
						// if the pixel bit is '1' and the same gfx bit is '1', we have a pixel collision, set the VF register
						V[0xF] = 1;
					}
					// now XOR '1' with the gfx register to get the final pixel value to draw (on or off)
					//   this effectively makes the screen pixel '1' or on if it was '0' of off
					//   OR we make it '0' if it was '1' because we had a pixel collision
					gfx[(x + xline) + ((y + yline) * 64)] ^= 1;
				}
			}
		}
		drawFlag = true;
		pc += 2; 
		break;
	
	// opcodes 0xEX?? -> Low byte determines the operation 
	case 0xE000:
		switch (opcode & 0x00FF) {
			
		// opcode 0xEX9E -> Skips the next instruction if the key stored in VX is pressed.
		case 0x009E:
			unsigned short store_key = V[(opcode & 0x0F00) >> 8];
			if (store_key <= 0xF) {
				if (key[store_key] == 1) {
					pc += 4;
				}
				else {
					pc += 2;
				}
			}
			else {
				char msg[] = "Key stored in V[%i] is outside of the hex bounds.";
				debug_fmt_msg(msg, store_key);
				debug_simple_msg("Illegal reference attempted on key[] array!");
				exit(0);
			}
			break;

		// opcode 0xEXA1 -> Skips the next instruction if the key stored in VX isn't pressed.
		case 0x00A1:
			unsigned short store_key = V[(opcode & 0x0F00) >> 8];
			if (store_key <= 0xF) {
				if (key[store_key] == 0) {
					pc += 4;
				}
				else {
					pc += 2;
				}
			}
			else {
				char msg[] = "Key stored in V[%i] is outside of the hex bounds.";
				debug_fmt_msg(msg, store_key);
				debug_simple_msg("Illegal reference attempted on key[] array!");
				exit(1);
			}
			break;

		default:
			char msg[] = "Unknown opcode %X";
			debug_fmt_msg(msg, opcode);
			debug_simple_msg("Bad program - exiting!");
			exit(1);
		}

	// opcodes 0xFX?? -> Last byte determines operation
	case 0xF000:
		switch (opcode & 0x00FF) {

		// opcode 0xFX07 -> Sets VX to the value of the delay timer.
		case 0x0007:
			V[(opcode & 0x0F00) >> 8] = delay_timer;
			pc += 2;
			break;

		// opcode 0xFX0A -> A key press is awaited, and then stored in VX. 
		//					(Blocking Operation. All instruction halted until next key event)
		case 0x000A:
			bool isKeyPressed = false;
			for (int i = 0; i < 16; ++i) {
				if (key[i] == 1) {
					isKeyPressed = true;
					V[(opcode & 0x0F00) >> 8] = i;
					break;
				}
			}
			if (!isKeyPressed) {
				return;  // skip the rest of the emulation cycle and try again (wait on the key)
			}
			pc += 2;
			break;

		// opcode 0xFX15 -> Sets the delay timer to VX.	
		case 0x0015:
			delay_timer = V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;

		// opcode 0xFX18 -> Sets the sound timer to VX.
		case 0x0018: 
			sound_timer = V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;

		// opcode 0xFX1E -> Adds VX to I.
		//
		// Per Wikipedia footnote (3)
		// VF is set to 1 when there is a range overflow (I+VX>0xFFF), and to 0 when there isn't. 
		// This is an undocumented feature of the CHIP-8 and used by the Spacefight 2091! game.
		case 0x001E:
			if ((I + V[(opcode & 0x0F00) >> 8]) > 0xFFF) {
				V[0xF] = 1;
			}
			else {
				V[0xF] = 0;
			}
			I += V[(opcode & 0x0F00) >> 8];  // now just add them and store in I
			pc += 2;
			break;

		// opcode 0xFX29 -> Sets I to the location of the sprite for the character in VX. 
		//                  Characters 0-F (in hexadecimal) are represented by a 4x5 font.
		case 0x0029:
			// get the character from V[(opcode & 0x0F00) >> 8]
			// we need to get the sprite data for this from the fontset memory
			// each character is 5 bytes wide (4x5)

			// implementation of this is multiple what V[X] points to by 5 (0 * 5 = 0, 1 * 5 = 5, 2 * 5 = 10, etc...)
			//  this will skip ahead the correct number of locations (bytes) in the fontset array to the start of the correct hex character
			I = V[(opcode & 0x0F00) >> 8] * 0x5;
			pc += 2;
			break;

		// opcode 0xFX33 -> Stores the binary-coded decimal representation of VX, 
		//                  with the most significant of three digits at the address in I, 
		//                  the middle digit at I plus 1, and the least significant digit at I plus 2. 
		//                  (In other words, take the decimal representation of VX, 
		//                    place the hundreds digit in memory at location in I, the tens digit at location I+1, 
		//                    and the ones digit at location I+2.)
		case 0x0033:
			unsigned short bin_val = V[(opcode & 0x0F00) >> 8];
			unsigned short dec_val = 0;
			for (int i = 0; i < 8; ++i) {
				if (bin_val & 0x00000001) {
					dec_val += 2 ^ i;
				}
				bin_val >>= 1;
			}

			// TODO:  now add to I at the appropriate locations

			pc += 2;
			break;

		default:
			char msg[] = "Unknown opcode %X";
			debug_fmt_msg(msg, opcode);
			debug_simple_msg("Bad program - exiting!");
			exit(1);
		}



	default: 
		char msg[] = "Unknown opcode: 0x%X";
		debug_fmt_msg(msg, opcode);
		break;
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
			debug_simple_msg("BEEP!");
			--sound_timer;
		}
	}
}

bool chip8::loadApp(char *filename)
{
	// initialize chip8
	initialize();

	debug_fmt_msg("Loading filename: %s", filename);

	// open file for reaching in binary mode (do not map end of line)
	FILE *ptrFile = NULL;
	fopen_s(&ptrFile, filename, "rb");
	if (ptrFile == NULL)
	{
		debug_fmt_msg("Filename %s does not exist!", filename);
		return false;
	}

	// get the buffer size (size of file)
	fseek(ptrFile, 0, SEEK_END);
	long bufferSize = ftell(ptrFile);
	rewind(ptrFile);
	debug_fmt_msg("Filesize found to be: %d", bufferSize);

	// buffer should have 'bufferSize' number of bytes for the system
	char *buffer = (char*)malloc(sizeof(char) * bufferSize);
	if (buffer == NULL)
	{
		debug_simple_msg("Memory allocation error!");
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
	DBOUT(message);
	DBOUT("\n");
}

template <typename T>
void chip8::debug_fmt_msg(char formatted_message[], T object)
{
	const size_t buf_size = ((sizeof(formatted_message) / sizeof(char)) * sizeof(formatted_message)) * 2;
	char buffer[buf_size];
	sprintf_s(buffer, formatted_message, object);
	DBOUT(buffer);
	DBOUT("\n");
}

