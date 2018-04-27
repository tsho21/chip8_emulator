#pragma once

unsigned short opcode_0x00E0() {
	for (int i = 0; i < (64 * 32); ++i) {
		gfx[i] = 0x0;
	}
	drawFlag = true;
	pc += 2;
}