#include "Input.h"

namespace Chip8 {

    // set the keys based on the key pressed
    void keyboardDown(unsigned char key, int x, int y)
    {
        // exit = esc key = 27
        if (key == 27) {
            emu_chip.debug_simple_msg("ESC key pressed - exiting program!");
            exit(0);
        }

        if (key == KEY_0) {
            emu_chip.key[0x0] = 1;
        }
        else if (key == KEY_1) {
            emu_chip.key[0x1] = 1;
        }
        else if (key == KEY_2) {
            emu_chip.key[0x2] = 1;
        }
        else if (key == KEY_3) {
            emu_chip.key[0x3] = 1;
        }
        else if (key == KEY_4) {
            emu_chip.key[0x4] = 1;
        }
        else if (key == KEY_5) {
            emu_chip.key[0x5] = 1;
        }
        else if (key == KEY_6) {
            emu_chip.key[0x6] = 1;
        }
        else if (key == KEY_7) {
            emu_chip.key[0x7] = 1;
        }
        else if (key == KEY_8) {
            emu_chip.key[0x8] = 1;
        }
        else if (key == KEY_9) {
            emu_chip.key[0x9] = 1;
        }
        else if (key == KEY_A) {
            emu_chip.key[0xA] = 1;
        }
        else if (key == KEY_B) {
            emu_chip.key[0xB] = 1;
        }
        else if (key == KEY_C) {
            emu_chip.key[0xC] = 1;
        }
        else if (key == KEY_D) {
            emu_chip.key[0xD] = 1;
        }
        else if (key == KEY_E) {
            emu_chip.key[0xE] = 1;
        }
        else if (key == KEY_F) {
            emu_chip.key[0xF] = 1;
        }
    }

    // unset the keys when the key is released
    void keyboardUp(unsigned char key, int x, int y)
    {
        if (key == KEY_0) {
            emu_chip.key[0x0] = 0;
        }
        else if (key == KEY_1) {
            emu_chip.key[0x1] = 0;
        }
        else if (key == KEY_2) {
            emu_chip.key[0x2] = 0;
        }
        else if (key == KEY_3) {
            emu_chip.key[0x3] = 0;
        }
        else if (key == KEY_4) {
            emu_chip.key[0x4] = 0;
        }
        else if (key == KEY_5) {
            emu_chip.key[0x5] = 0;
        }
        else if (key == KEY_6) {
            emu_chip.key[0x6] = 0;
        }
        else if (key == KEY_7) {
            emu_chip.key[0x7] = 0;
        }
        else if (key == KEY_8) {
            emu_chip.key[0x8] = 0;
        }
        else if (key == KEY_9) {
            emu_chip.key[0x9] = 0;
        }
        else if (key == KEY_A) {
            emu_chip.key[0xA] = 0;
        }
        else if (key == KEY_B) {
            emu_chip.key[0xB] = 0;
        }
        else if (key == KEY_C) {
            emu_chip.key[0xC] = 0;
        }
        else if (key == KEY_D) {
            emu_chip.key[0xD] = 0;
        }
        else if (key == KEY_E) {
            emu_chip.key[0xE] = 0;
        }
        else if (key == KEY_F) {
            emu_chip.key[0xF] = 0;
        }
    }
}