#pragma once

/**

 --- I Probably don't need this - will do a flat array and map the functions by their enum number for the opcode

*/

typedef struct Key {
	chip8::Opcode opcode;
} Key;

typedef struct Value {
	bool(*opcode_impl)();
} Value;

struct Map {
	Key key;
	Value value;
};
