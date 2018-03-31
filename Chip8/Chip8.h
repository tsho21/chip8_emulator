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

	// current opcode
	unsigned short opcode;

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
};