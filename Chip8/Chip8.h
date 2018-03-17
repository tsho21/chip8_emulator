#pragma once

class chip8 {
public:

	// current opcode
	unsigned short opcode;

	// memory size
	static unsigned const short MEMORY_SIZE = 4096;

	// memory
	//  0x00 -> 0x50 = font set
	//  0x200 = start of program memory
	//  
	unsigned char memory[4096];

	// registers
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

	// keypad (hex-based), 0 - F
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
	bool loadGame(char *filename);
	void setKeys();

	template <typename T>
	void debug_fmt_msg(char *formatted_message, T values);
	void debug_simple_msg(char *message);
};