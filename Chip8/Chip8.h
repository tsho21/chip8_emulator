#pragma once
#ifndef _CHIP8_H
#define _CHIP8_H

#include "Common.h"

// keypad (hex-based), 0 - F | 16 total keys as shown below (left side of standard keyboard)
//	1	49		2	50		3	51		4	52
//	q	81		w	87		e	69		r	82
//	a	65		s	83		d	68		f	70
//	z	90		x	88		c	67		v	86
//
//	x = key '0'
#define KEY_1	49
#define KEY_2	50
#define	KEY_3	51
#define KEY_4	52
#define KEY_5	113
#define KEY_6	119
#define KEY_7	101
#define KEY_8	114
#define KEY_9	97
#define KEY_A	115
#define KEY_B	100
#define KEY_C	102
#define KEY_D	122
#define KEY_0	120
#define KEY_E	99
#define KEY_F	118

#define GFX_WIDTH 64
#define GFX_HEIGHT 32
#define GFX_SIZE ((GFX_WIDTH) * (GFX_HEIGHT))

#define TARGET_CLOCK_SPEED 540
#define SCREEN_REFRESH_RATE 60

class chip8 {
public:

	// sprite width = 8 pixels, each pixel is 1 bit (on=1, off=0)
	static const uint8 SPRITE_WIDTH = 8;

	// memory
	//  0x00 -> 0x50 = font set
	//  0x200 = start of program memory
    static const uint16 MEMORY_SIZE = 4096;
	uint8 memory[MEMORY_SIZE];

	// 1 uint8 (8 bit) data registers
    static const uint16 REGISTER_COUNT = 16;
	uint8 V[REGISTER_COUNT];

	// index (I) and program counter (pc)
	uint16 I;
	uint16 pc;

	// pixel state (1=on=white,0=off=black)
	uint8 gfx[GFX_SIZE];

	// timers
	uint8 delay_timer;
	uint8 sound_timer;

	// stack and stack pointer (sp)
    static const uint16 STACK_LEVELS = 16;
	uint16 stack[STACK_LEVELS];

    // stack pointer
	uint16 sp;

	// key states
    static const uint16 KEY_STATES = 16;
	uint8 key[KEY_STATES];

	// draw flag (if we draw next cycle)
	uint16 drawFlag;

	// font set
	uint8 chip8_fontset[80] =
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0  addr 0x00
		0x20, 0x60, 0x20, 0x20, 0x70, // 1  addr 0x05
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2  addr 0x0A
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3  addr 0x0F
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4  addr 0x14
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5  addr 0x19
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6  addr 0x1E
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7  addr 0x23
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8  addr 0x28
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9  addr 0x2D
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A  addr 0x32
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B  addr 0x37
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C  addr 0x3C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D  addr 0x41
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E  addr 0x46
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F  addr 0x4B
	};

	chip8();
	~chip8();

	void initialize();
	void emulateCycle();
	bool loadApp(char *filename);
	void setKeys();
    void updateTimers();

	template <typename T>
	void debug_fmt_msg(char formatted_message[], T values);
	void debug_simple_msg(char *message);

	/**
	 * When adding opcodes:
	 *  1.  Add to enum 'opcode' before the last entry
	 *  2.  Add the declaration of the opcode routine under '// opcode routines'
	 *  3.  Add the address of the routine to the end of 'opcode_impls'
	 *  4.  Define the implementation of the opcode routine in Chip8.cpp
	*/

	// opcodes
	enum opcodes {
		INVALID_OPCODE = -1,
		_0x00E0 = 0,
		_0x00EE,
		_0x0NNN,
		_0x1NNN,
		_0x2NNN,
		_0x3XNN,
		_0x4XNN,
		_0x5XY0,
		_0x6XNN,
		_0x7XNN,
		_0x8XY0,
		_0x8XY1,
		_0x8XY2,
		_0x8XY3,
		_0x8XY4,
		_0x8XY5,
		_0x8XY6,
		_0x8XY7,
		_0x8XYE,
		_0x9XY0,
		_0xANNN,
		_0xBNNN,
		_0xCXNN,
		_0xDXYN,
		_0xEX9E,
		_0xEXA1,
		_0xFX07,
		_0xFX0A,
		_0xFX15,
		_0xFX18,
		_0xFX1E,
		_0xFX29,
		_0xFX33,
		_0xFX55,
		_0xFX65,
		NUMBER_OF_OPCODES
	};

	typedef enum opcodes Opcode;
	

	// translate opcode
	Opcode translate_opcode(uint16);

	// opcode routines
	bool opcode_0x00E0(uint16);
	bool opcode_0x00EE(uint16);
	bool opcode_0x0NNN(uint16);
	bool opcode_0x1NNN(uint16);
	bool opcode_0x2NNN(uint16);
	bool opcode_0x3XNN(uint16);
	bool opcode_0x4XNN(uint16);
	bool opcode_0x5XY0(uint16);
	bool opcode_0x6XNN(uint16);
	bool opcode_0x7XNN(uint16);
	bool opcode_0x8XY0(uint16);
	bool opcode_0x8XY1(uint16);
	bool opcode_0x8XY2(uint16);
	bool opcode_0x8XY3(uint16);
	bool opcode_0x8XY4(uint16);
	bool opcode_0x8XY5(uint16);
	bool opcode_0x8XY6(uint16);
	bool opcode_0x8XY7(uint16);
	bool opcode_0x8XYE(uint16);
	bool opcode_0x9XY0(uint16);
	bool opcode_0xANNN(uint16);
	bool opcode_0xBNNN(uint16);
	bool opcode_0xCXNN(uint16);
	bool opcode_0xDXYN(uint16);
	bool opcode_0xEX9E(uint16);
	bool opcode_0xEXA1(uint16);
	bool opcode_0xFX07(uint16);
	bool opcode_0xFX0A(uint16);
	bool opcode_0xFX15(uint16);
	bool opcode_0xFX18(uint16);
	bool opcode_0xFX1E(uint16);
	bool opcode_0xFX29(uint16);
	bool opcode_0xFX33(uint16);
	bool opcode_0xFX55(uint16);
	bool opcode_0xFX65(uint16);

	// opcode information
	typedef bool(chip8::*opcode_impl)(uint16);
	struct opcode_info {
		Opcode opcode;
		char *description;
		opcode_impl executor;
	};

	opcode_info opcodes[NUMBER_OF_OPCODES] = {

		{ _0x00E0, "Clears the screen.",                                                            &chip8::opcode_0x00E0 },
		{ _0x00EE, "Returns from a subroutine.",                                                    &chip8::opcode_0x00EE },
		{ _0x0NNN, "Calls RCA 1802 program at address NNN. Not necessary for most ROMs.",           &chip8::opcode_0x0NNN },
		{ _0x1NNN, "Jump to address specified in 'NNN'.",                                           &chip8::opcode_0x1NNN },
		{ _0x2NNN, "all subroutine (subroutine will return).",                                      &chip8::opcode_0x2NNN },
		{ _0x3XNN, "Skip the next instruction if VX equals NN.",                                    &chip8::opcode_0x3XNN },
		{ _0x4XNN, "Skips the next instruction if VX doesn't equal NN.",                            &chip8::opcode_0x4XNN },
		{ _0x5XY0, "Skips the next instruction if VX equals VY.",                                   &chip8::opcode_0x5XY0 },
		{ _0x6XNN, "Sets VX to NN.",                                                                &chip8::opcode_0x6XNN },
		{ _0x7XNN, "Adds NN to VX. (Carry flag is not changed).",                                   &chip8::opcode_0x7XNN },
		{ _0x8XY0, "Sets VX to the value of VY.",                                                   &chip8::opcode_0x8XY0 },
		{ _0x8XY1, "Sets VX to VX or VY (Bitwise OR operation).",                                   &chip8::opcode_0x8XY1 },
		{ _0x8XY2, "Sets VX to VX and VY. (Bitwise AND operation).",                                &chip8::opcode_0x8XY2 },
		{ _0x8XY3, "Sets VX to VX xor VY.",                                                         &chip8::opcode_0x8XY3 },
		{ _0x8XY4, "Adds VY to VX. VF=1 if carry, VF=0 if no carry.",                               &chip8::opcode_0x8XY4 },
		{ _0x8XY5, "VY is subtracted from VX. VF=0 if borrow, VF=1 if no borrow.",                  &chip8::opcode_0x8XY5 },
		{ _0x8XY6, "Shifts VY right by one. VX=new VY value, VF=LSB of VY before shift.",           &chip8::opcode_0x8XY6 },
		{ _0x8XY7, "Sets VX to VY minus VX. VF=0 if borrow, VF=1 if no borrow.",                    &chip8::opcode_0x8XY7 },
		{ _0x8XYE, "Shifts VY left by one. VX=new VY value, VF=MSB of VY before shift.",            &chip8::opcode_0x8XYE },
		{ _0x9XY0, "Skips the next instruction if VX doesn't equal VY.",                            &chip8::opcode_0x9XY0 },
		{ _0xANNN, "Sets I to the address NNN.",                                                    &chip8::opcode_0xANNN },
		{ _0xBNNN, "Jumps to the address NNN plus V0.",                                             &chip8::opcode_0xBNNN },
		{ _0xCXNN, "Sets VX=result of bitwise and operation between random(0 to 255) and NN.",      &chip8::opcode_0xCXNN },
		{ _0xDXYN, "Draw a sprite on screen (sprite=8 pixels wide, (opcode & 0x000F) pixels high)", &chip8::opcode_0xDXYN },
		{ _0xEX9E, "Skips the next instruction if the key stored in VX is pressed.",                &chip8::opcode_0xEX9E },
		{ _0xEXA1, "Skips the next instruction if the key stored in VX isn't pressed.",             &chip8::opcode_0xEXA1 },
		{ _0xFX07, "Sets VX to the value of the delay timer.",                                      &chip8::opcode_0xFX07 },
		{ _0xFX0A, "A key press is awaited, and then stored in VX. Blocking-Waits for input.",      &chip8::opcode_0xFX0A },
		{ _0xFX15, "Sets the delay timer to VX.",                                                   &chip8::opcode_0xFX15 },
		{ _0xFX18, "Sets the sound timer to VX.",                                                   &chip8::opcode_0xFX18 },
		{ _0xFX1E, "Adds VX to I. VF=1 if range overflow (I+VX>0xFFF), VF=0 if no range overflow",  &chip8::opcode_0xFX1E },
		{ _0xFX29, "Sets I to the location of the sprite for the character in VX. Hex Chars 4x5",   &chip8::opcode_0xFX29 },
		{ _0xFX33, "I=MSD(decimal(VX)), I+1=mid(decimal(VX)), I+2=LSB(decimal(VX)).",               &chip8::opcode_0xFX33 },
		{ _0xFX55, "Stores V0 to VX (inclusive) starting memory[I]. I+=1 for each value written.",  &chip8::opcode_0xFX55 },
		{ _0xFX65, "Dump to V0 to VX (inclusive) starting memory[I]. I+=1 for each value written.", &chip8::opcode_0xFX65 }
	};
};

#endif