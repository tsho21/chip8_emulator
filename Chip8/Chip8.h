#pragma once

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
#define KEY_5	81
#define KEY_6	87
#define KEY_7	69
#define KEY_8	82
#define KEY_9	65
#define KEY_A	83
#define KEY_B	68
#define KEY_C	70
#define KEY_D	90
#define KEY_0	88
#define KEY_E	67
#define KEY_F	86

class chip8 {
public:

	// memory size
	static unsigned const short MEMORY_SIZE = 4096;

	// sprite width = 8 pixels, each pixel is 1 bit (on=1, off=0)
	static unsigned const short SPRITE_WIDTH = 8;

	// memory
	//  0x00 -> 0x50 = font set
	//  0x200 = start of program memory
	//  
	unsigned char memory[4096];

	// 1 byte (8 bit) data registers  - V[0xF] or V[15] seems to be contextual -> used for bit collision, carry, borrow
	unsigned char V[16];

	// index (I) and program counter (pc)
	unsigned short I;
	unsigned short pc;

	// pixel state (1=on=white,0=off=black)
	unsigned char gfx[64 * 32];

	// timers
	unsigned char delay_timer;
	unsigned char sound_timer;

	// stack levels (const)
	static unsigned const short STACK_SIZE = 16;

	// stack and stack pointer (sp)
	unsigned short stack[STACK_SIZE];
	unsigned short sp;

	// key states
	unsigned char key[16];

	// draw flag (if we draw next cycle)
	unsigned short drawFlag;

	// font set
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

	chip8();
	~chip8();

	void initialize();
	void emulateCycle();
	bool loadApp(char *filename);
	void setKeys();

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
	Opcode translate_opcode(unsigned short);

	// opcode routines
	bool opcode_0x00E0(unsigned short);
	bool opcode_0x00EE(unsigned short);
	bool opcode_0x0NNN(unsigned short);
	bool opcode_0x1NNN(unsigned short);
	bool opcode_0x2NNN(unsigned short);
	bool opcode_0x3XNN(unsigned short);
	bool opcode_0x4XNN(unsigned short);
	bool opcode_0x5XY0(unsigned short);
	bool opcode_0x6XNN(unsigned short);
	bool opcode_0x7XNN(unsigned short);
	bool opcode_0x8XY0(unsigned short);
	bool opcode_0x8XY1(unsigned short);
	bool opcode_0x8XY2(unsigned short);
	bool opcode_0x8XY3(unsigned short);
	bool opcode_0x8XY4(unsigned short);
	bool opcode_0x8XY5(unsigned short);
	bool opcode_0x8XY6(unsigned short);
	bool opcode_0x8XY7(unsigned short);
	bool opcode_0x8XYE(unsigned short);
	bool opcode_0x9XY0(unsigned short);
	bool opcode_0xANNN(unsigned short);
	bool opcode_0xBNNN(unsigned short);
	bool opcode_0xCXNN(unsigned short);
	bool opcode_0xDXYN(unsigned short);
	bool opcode_0xEX9E(unsigned short);
	bool opcode_0xEXA1(unsigned short);
	bool opcode_0xFX07(unsigned short);
	bool opcode_0xFX0A(unsigned short);
	bool opcode_0xFX15(unsigned short);
	bool opcode_0xFX18(unsigned short);
	bool opcode_0xFX1E(unsigned short);
	bool opcode_0xFX29(unsigned short);
	bool opcode_0xFX33(unsigned short);
	bool opcode_0xFX55(unsigned short);
	bool opcode_0xFX65(unsigned short);

	// opcode routine list
	typedef bool(chip8::*opcode_impl)(unsigned short);
	opcode_impl opcode_impls[NUMBER_OF_OPCODES] = {
		&opcode_0x00E0,
		&opcode_0x00EE,
		&opcode_0x0NNN,
		&opcode_0x1NNN,
		&opcode_0x2NNN,
		&opcode_0x3XNN,
		&opcode_0x4XNN,
		&opcode_0x5XY0,
		&opcode_0x6XNN,
		&opcode_0x7XNN,
		&opcode_0x8XY0,
		&opcode_0x8XY1,
		&opcode_0x8XY2,
		&opcode_0x8XY4,
		&opcode_0x8XY5,
		&opcode_0x8XY6,
		&opcode_0x8XY7,
		&opcode_0x8XYE,
		&opcode_0x9XY0,
		&opcode_0xANNN,
		&opcode_0xBNNN,
		&opcode_0xCXNN,
		&opcode_0xDXYN,
		&opcode_0xEX9E,
		&opcode_0xEXA1,
		&opcode_0xFX07,
		&opcode_0xFX0A,
		&opcode_0xFX15,
		&opcode_0xFX18,
		&opcode_0xFX1E,
		&opcode_0xFX29,
		&opcode_0xFX33,
		&opcode_0xFX55,
		&opcode_0xFX65
	};
};