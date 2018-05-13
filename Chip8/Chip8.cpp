#include "stdio.h"  // debug and loader only
#include "stdlib.h"
#include "Common.h"
#include "Chip8.h"
#include "Debug.h"

chip8::chip8()
{
	// empty constructor
}

chip8::~chip8()
{
	// empty destructor
}

chip8::Opcode chip8::translate_opcode(uint16 opcode) {
	switch (opcode & 0xF000) {
	case 0x0000:
		switch (opcode & 0x0FFF) {
		case 0x00E0:
			return _0x00E0;
			break;
		case 0x00EE:
			return _0x00EE;
			break;
		default:
			return _0x0NNN;
			break;
		}
	case 0x1000:
		return _0x1NNN;
		break;
	case 0x2000:
		return _0x2NNN;
		break;
	case 0x3000:
		return _0x3XNN;
		break;
	case 0x4000:
		return _0x4XNN;
		break;
	case 0x5000:
		return _0x5XY0;
		break;
	case 0x6000:
		return _0x6XNN;
		break;
	case 0x7000:
		return _0x7XNN;
		break;
	case 0x8000:
		switch (opcode & 0x000F) {
		case 0x0000:
			return _0x8XY0;
			break;
		case 0x0001:
			return _0x8XY1;
			break;
		case 0x0002:
			return _0x8XY2;
			break;
		case 0x0003:
			return _0x8XY3;
			break;
		case 0x0004:
			return _0x8XY4;
			break;
		case 0x0005:
			return _0x8XY5;
			break;
		case 0x0006:
			return _0x8XY6;
			break;
		case 0x0007:
			return _0x8XY7;
			break;
		case 0x000E:
			return _0x8XYE;
			break;
		}
	case 0x9000:
		return _0x9XY0;
		break;
	case 0xA000:
		return _0xANNN;
		break;
	case 0xB000:
		return _0xBNNN;
		break;
	case 0xC000:
		return _0xCXNN;
		break;
	case 0xD000:
		return _0xDXYN;
		break;
	case 0xE000:
		switch (opcode & 0x00FF) {
		case 0x009E:
			return _0xEX9E;
			break;
		case 0x00A1:
			return _0xEX9E;
			break;
		}
	case 0xF000:
		switch (opcode & 0x00FF) {
		case 0x0007:
			return _0xFX07;
			break;
		case 0x000A:
			return _0xFX0A;
			break;
		case 0x0015:
			return _0xFX15;
			break;
		case 0x0018:
			return _0xFX18;
			break;
		case 0x001E:
			return _0xFX1E;
			break;
		case 0x0029:
			return _0xFX29;
			break;
		case 0x0033:
			return _0xFX33;
			break;
		case 0x0055:
			return _0xFX55;
			break;
		case 0x0065:
			return _0xFX65;
			break;
		}

	default:
		return INVALID_OPCODE;
		break;
	}
}

void chip8::initialize()
{
	// init registers and memory once
	pc = 0x200;			// program counter always starts at 0x200
	I = 0;				// reset index register
	sp = 0;				// reset stack pointer

	 // clear display
    memset(gfx, 0, sizeof(uint8) * (GFX_WIDTH * GFX_HEIGHT));

	// clear stack
    memset(stack, 0, STACK_LEVELS); 

	// clear registers V0 through VF
    memset(V, 0, sizeof(uint8) * REGISTER_COUNT); 

	// clear keyset
    memset(key, 0, sizeof(uint8) * KEY_STATES);

	// clear memory
    memset(memory, 0, sizeof(uint8) * MEMORY_SIZE);

	// load fontset
    memcpy(memory, chip8_fontset, 80);

	// reset timers
	delay_timer = 0;
	sound_timer = 0;

	// signal a screen clear
	drawFlag = true;
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
	uint16 raw_opcode = memory[pc] << 8 | memory[pc + 1];

	// decode opcode
	Opcode opcode = translate_opcode(raw_opcode);  // this should really just be identifed as a hash into a bucket map below

	if (opcode == INVALID_OPCODE) {
		debug_fmt_msg("Invalid Opcode parsed from loaded file:  %i", raw_opcode);
		getchar();
		exit(1);
	}

	// get the implementation:  Uses the numerical behavior of enum values to get the correct opcode impl via it's index
	opcode_impl func = opcodes[opcode].executor;

	// execute the opcode
	bool result = (this->*func)(raw_opcode);
	if (!result) {
		debug_simple_msg("Unexepcted result from opcode execution, exiting...");
		getchar();
		exit(1);
	}
}

bool chip8::loadApp(char *filename)
{
	// initialize chip8
	initialize();

	debug_fmt_msg("Loading filename: %s", filename);

#ifdef DEBUG
    if (filename == NULL && DEBUG) {
        filename = DEFAULT_APP;
        debug_fmt_msg("DEBUG => Loading filename: %s", filename);
    }
#endif

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

    // check the file won't overrun the memory
    if (bufferSize > (MEMORY_SIZE - 512)) {
        debug_fmt_msg("Filesize too large for available RAM: %l", bufferSize);
    }

	// buffer should have 'bufferSize' number of bytes for the system
	char *buffer = (char*)malloc(sizeof(char) * bufferSize);
	if (buffer == NULL)
	{
		debug_simple_msg("Memory allocation error!");
	}

	// read the bytes into the buffer 
	size_t bytesread = fread(buffer, 1, bufferSize, ptrFile);
	
	// check that they were read
	if (bytesread != bufferSize) {
		debug_simple_msg("Read error, too many or too few bytes were read - check the file.");
		getchar();
		exit(1);
	}

	// program or game is loaded into memory starting at location 0x200 (512 in decimal)
	for (int i = 0; i < bufferSize; ++i)
	{
		memory[i + 512] = buffer[i];
	}

    fclose(ptrFile);
    free(buffer);
	return true;
}

void chip8::updateTimers() 
{
    // update delay timer
    if (delay_timer > 0)
    {
        --delay_timer;
    }

    // update sound timers
    if (sound_timer > 0)
    {
        if (sound_timer == 1)
        {
            debug_simple_msg("BEEP!");
            --sound_timer;
        }
    }
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
	const size_t buf_size = 512;  // TODO:  Needs fixed later to get the length needed
	char buffer[buf_size];
	sprintf_s(buffer, formatted_message, object);
	DBOUT(buffer);
	DBOUT("\n");
}

/**
 * Opcode Routines
 * 
*/

// opcode 0x00E0 -> Clears the screen
bool chip8::opcode_0x00E0(uint16 opcode) {
    memset(gfx, 0, sizeof(uint8) * (GFX_WIDTH * GFX_HEIGHT));
	drawFlag = true;
	pc += 2;
	return true; 
}

// opcode 0x00EE -> Returns from a subroutine
bool chip8::opcode_0x00EE(uint16 opcode) {
	// pop the stack and return to where the pc pointer was
	--sp;
	pc = stack[sp];  // return to the point of function call
	pc += 2;    // increase the pc to the next instruction after the function call
	return true; 
}

// opcode 0x0NNN -> Calls RCA 1802 program at address NNN. Not necessary for most ROMs.
bool chip8::opcode_0x0NNN(uint16 opcode) {
	stack[sp] = pc;  // store the current pc on the stack
	++sp;			 // increment stack pointer
	pc = (opcode & 0x0FFF);  // set the pc to the address specified in the opcode (NNN part of 0x0NNN)
	return true; 
}

// opcodes 0x1NNN -> jump to address specified in 'NNN' 
bool chip8::opcode_0x1NNN(uint16 opcode) {
	pc = (opcode & 0x0FFF); // jump to address stored in NNN
	return true; 
}

// opcodes 0x2NNN -> call subroutine (subroutine will return)
bool chip8::opcode_0x2NNN(uint16 opcode) {
	stack[sp] = pc;    // store the current pc in the stack
	++sp;			   // increase the stack pointer to next avail location
	pc = opcode & 0x0FFF;	// set the pc to the address specified in the opcode (NNN part of 0x2NNN)
	return true; 
}

// opcode 0x3XNN -> Skip the next instruction if VX equals NN
bool chip8::opcode_0x3XNN(uint16 opcode) {
	if ((V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))) {
		pc += 4;   // skip the next instruction 
	}
	else {
		pc += 2;   // otherwise, just go to the next instruction
	}
	return true; 
}

// opcode 0x4XNN -> Skips the next instruction if VX doesn't equal NN.
bool chip8::opcode_0x4XNN(uint16 opcode) {
	if ((V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))) {
		pc += 4;  // skip the next instruction
	}
	else {
		pc += 2;  // otherwise, just go to the next instruction
	}
	return true; 
}

// opcode 0x5XY0 -> Skips the next instruction if VX equals VY. 
bool chip8::opcode_0x5XY0(uint16 opcode) {
	if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
		pc += 4;  // skip the next instruction
	}
	else {
		pc += 2;  // otherwise, just go to the next instruction
	}
	return true; 
}

// opcode 0x6XNN -> Sets VX to NN.
bool chip8::opcode_0x6XNN(uint16 opcode) {
	V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
	pc += 2;
	return true; 
}

// opcode 0x7XNN -> Adds NN to VX. (Carry flag is not changed)
bool chip8::opcode_0x7XNN(uint16 opcode) {
	V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
	pc += 2;
	return true; 
}

// opcode 0x8XY0 -> Sets VX to the value of VY
bool chip8::opcode_0x8XY0(uint16 opcode) {
	V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
	pc += 2;
	return true; 
}

// opcode 0x8XY1 -> Sets VX to VX or VY (Bitwise OR operation)
bool chip8::opcode_0x8XY1(uint16 opcode) {
	V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
	pc += 2;
	return true;
}

// opcode 0x8XY2 -> Sets VX to VX and VY. (Bitwise AND operation)
bool chip8::opcode_0x8XY2(uint16 opcode) {
	V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
	pc += 2;
	return true; 
}

// opcode 0x8XY3 -> Sets VX to VX xor VY.
bool chip8::opcode_0x8XY3(uint16 opcode) {
	V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
	pc += 2;
	return true; 
}

// opcode 0x8XY4 -> Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't
bool chip8::opcode_0x8XY4(uint16 opcode) {
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
	return true; 
}

// opcode 0x8XY5 -> VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
bool chip8::opcode_0x8XY5(uint16 opcode) {
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
	return true; 
}

// opcode 0x8XY6 -> Shifts VY right by one and copies the result to VX. 
//					VF is set to the value of the least significant bit of VY before the shift
bool chip8::opcode_0x8XY6(uint16 opcode) {
	V[0xF] = V[(opcode & 0x00F0) >> 4] & 0x01;
	V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x00F0) >> 4] >>= 1);
	pc += 2;
	return true; 
}

// opcode 0x8XY7 -> Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
bool chip8::opcode_0x8XY7(uint16 opcode) {
	// if we're subtracting VX from VY, if VX is larger than VY, there will be a borrow
	if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]) {
		V[0xF] = 0;
	}
	else {
		V[0xF] = 1;
	}
	V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
	pc += 2;
	return true; 
}

// opcode 0x8XYE -> Shifts VY left by one and copies the result to VX. 
//					VF is set to the value of the most significant bit of VY before the shift
bool chip8::opcode_0x8XYE(uint16 opcode) {
	V[0xF] = V[(opcode & 0x00F0) >> 4] & 0x80;   // msb = 0b10000000 = 0x80
	V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x00F0) >> 4] <<= 1);
	pc += 2;
	return true; 
}

// opcode 0x9XY0 -> Skips the next instruction if VX doesn't equal VY.
bool chip8::opcode_0x9XY0(uint16 opcode) {
	if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
		pc += 4;  // skip next instruction
	}
	else {
		pc += 2;
	}
	return true; 
}

// opcode 0xANNN -> Sets I to the address NNN.
bool chip8::opcode_0xANNN(uint16 opcode) {
	I = opcode & 0x0FFF;    // - set I to the NNN part of the opcode
	pc += 2;                // - move the program counter by 2 for next opcode
	return true; 
}

// opcode 0xBNNN -> Jumps to the address NNN plus V0.
bool chip8::opcode_0xBNNN(uint16 opcode) {
	pc = V[0x0] + (opcode & 0x0FFF);
	return true; 
}

// opcode 0xCXNN -> Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
bool chip8::opcode_0xCXNN(uint16 opcode) {
	V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) && (opcode & 0x00FF);
	pc += 2;
	return true; 
}

// opcodes 0xDXYN ->  Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
bool chip8::opcode_0xDXYN(uint16 opcode) {
	uint16 row = V[(opcode & 0x0F00) >> 8];
	uint16 col = V[(opcode & 0x00F0) >> 4];
	uint16 n_bytes = opcode & 0x000F;
    uint16 byte_index;
    uint16 bit_index;
	uint16 byte;

	// reset register VF (this is the drawFlag and pixel collision register - status register)
	V[0xF] = 0;

	// loop through the height of the sprite
	for (byte_index = 0; byte_index < n_bytes; ++byte_index) {

		// get all bytes of the sprite to be drawn from memory - starting at I 
		byte = memory[I + byte_index];

		// scan through the bits of the sprite pixel obtained from memory (8 bits - use 0x10000000, or 0x80, and shift right to check)
		for (bit_index = 0; bit_index < SPRITE_WIDTH; ++bit_index) {

            // get the next bit to check
            uint8 draw_bit = NTH_BIT_OF_BYTE(byte, bit_index);

            // get the current pixel bit on screen
            uint8 *pixel_bit = &gfx[(row + bit_index) + ((col + byte_index) * GFX_WIDTH)];

            if (draw_bit == 1 && *pixel_bit == 1) {
                // collision detection
                V[0xF] = 1; 
            }

            // draw the pixel 
            *pixel_bit = *pixel_bit ^ draw_bit;
		}
	}
	drawFlag = true;
	pc += 2;
	return true; 
}

// opcode 0xEX9E -> Skips the next instruction if the key stored in VX is pressed.
bool chip8::opcode_0xEX9E(uint16 opcode) {
	uint16 store_key = V[(opcode & 0x0F00) >> 8];
	if (store_key <= 0xF) {
		if (key[store_key] == 1) {
			pc += 4;
		}
		else {
			pc += 2;
		}
		return true; 
	}
	else {
		char msg[] = "Key stored in V[%i] is outside of the hex bounds.";
		debug_fmt_msg(msg, store_key);
		debug_simple_msg("Illegal reference attempted on key[] array!");
		return false; 
	}
}

// opcode 0xEXA1 -> Skips the next instruction if the key stored in VX isn't pressed.
bool chip8::opcode_0xEXA1(uint16 opcode) {
	uint16 store_key = V[(opcode & 0x0F00) >> 8];
	if (store_key <= 0xF) {
		if (key[store_key] == 0) {
			pc += 4;
		}
		else {
			pc += 2;
		}
		return true; 
	}
	else {
		char msg[] = "Key stored in V[%i] is outside of the hex bounds.";
		debug_fmt_msg(msg, store_key);
		debug_simple_msg("Illegal reference attempted on key[] array!");
		return false;
	}
}

// opcode 0xFX07 -> Sets VX to the value of the delay timer.
bool chip8::opcode_0xFX07(uint16 opcode) {
	V[(opcode & 0x0F00) >> 8] = delay_timer;
	pc += 2;
	return true; 
}

// opcode 0xFX0A -> A key press is awaited, and then stored in VX. 
//					(Blocking Operation. All instruction halted until next key event)
bool chip8::opcode_0xFX0A(uint16 opcode) {
	bool isKeyPressed = false;
	for (int i = 0; i < 16; ++i) {
		if (key[i] == 1) {
			isKeyPressed = true;
			V[(opcode & 0x0F00) >> 8] = i;
			break;
		}
	}
	if (!isKeyPressed) {
		return true;  // skip the rest of the emulation cycle and try again (wait on the key)
	}
	pc += 2;
	return true;
}

// opcode 0xFX15 -> Sets the delay timer to VX.	
bool chip8::opcode_0xFX15(uint16 opcode) {
	delay_timer = V[(opcode & 0x0F00) >> 8];
	pc += 2;
	return true;
}

// opcode 0xFX18 -> Sets the sound timer to VX.
bool chip8::opcode_0xFX18(uint16 opcode) {
	sound_timer = V[(opcode & 0x0F00) >> 8];
	pc += 2;
	return true; 
}

// opcode 0xFX1E -> Adds VX to I.
//
// Per Wikipedia footnote (3)
// VF is set to 1 when there is a range overflow (I+VX>0xFFF), and to 0 when there isn't. 
// This is an undocumented feature of the CHIP-8 and used by the Spacefight 2091! game.
bool chip8::opcode_0xFX1E(uint16 opcode) {
	if ((I + V[(opcode & 0x0F00) >> 8]) > 0xFFF) {
		V[0xF] = 1;
	}
	else {
		V[0xF] = 0;
	}
	I += V[(opcode & 0x0F00) >> 8];  // now just add them and store in I
	pc += 2;
	return true; 
}

// opcode 0xFX29 -> Sets I to the location of the sprite for the character in VX. 
//                  Characters 0-F (in hexadecimal) are represented by a 4x5 font.
bool chip8::opcode_0xFX29(uint16 opcode) {
	// get the character from V[(opcode & 0x0F00) >> 8]
	// we need to get the sprite data for this from the fontset memory
	// each character is 5 bytes wide (4x5)

	// implementation of this is multiple what V[X] points to by 5 (0 * 5 = 0, 1 * 5 = 5, 2 * 5 = 10, etc...)
	//  this will skip ahead the correct number of locations (bytes) in the fontset array to the start of the correct hex character
	I = V[(opcode & 0x0F00) >> 8] * 0x5;
	pc += 2;
	return true; 
}

// opcode 0xFX33 -> Stores the binary-coded decimal representation of VX, 
//                  with the most significant of three digits at the address in I, 
//                  the middle digit at I plus 1, and the least significant digit at I plus 2. 
//                  (In other words, take the decimal representation of VX, 
//                    place the hundreds digit in memory at location in I, the tens digit at location I+1, 
//                    and the ones digit at location I+2.)
bool chip8::opcode_0xFX33(uint16 opcode) {
	uint16 bin_val = V[(opcode & 0x0F00) >> 8];
	uint16 dec_val = 0;
	for (int i = 0; i < 8; ++i) {
		if (bin_val & 0x00000001) {
			dec_val += 2 ^ i;
		}
		bin_val >>= 1;
	}

	// break dec_val down to the decimal places
	memory[I] = dec_val / 100;
	memory[I + 1] = (dec_val / 10) % 10;
	memory[I + 2] = (dec_val % 100) / 10;
	pc += 2;
	return true; 
}

// opcode 0xFX55 -> Stores V0 to VX (including VX) in memory starting at address I. I is increased by 1 for each value written.
bool chip8::opcode_0xFX55(uint16 opcode) {
	for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i) {
		memory[I + i] = V[i];
	}
	// I = I + X + 1
	I += ((opcode & 0x0F00) >> 8) + 1;
	pc += 2;
	return true; 
}

// opcode 0xFX65 -> Fills V0 to VX (including VX) with values from memory starting at address I. I is increased by 1 for each value written.
bool chip8::opcode_0xFX65(uint16 opcode) {
	for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i) {
		V[i] = memory[I + i];
	}
	// I = I + X + 1
	I += ((opcode & 0x0F00) >> 8) + 1;
	pc += 2;
	return true; 
}