#include "Chip8.h"
#include "GL/glut.h"
#include "Timer.h"

int modifier = 10;   // screen is too small without this

// window size
int display_width = GFX_WIDTH * modifier;
int display_height = GFX_HEIGHT * modifier;

// glut functions
void display();
void drawPixel(int, int);
void updateQuads();
void reshape_window(GLsizei w, GLsizei h);
void keyboardUp(unsigned char key, int x, int y);
void keyboardDown(unsigned char key, int x, int y);

// the chip to use 
chip8 emu_chip;

// cycle / clock timer
Timer timer;
int instruction_count = 0;

// Chip8 Graphics Setup Calls
void setupGraphics(int argc, char **argv)
{
	// Setup OpenGL
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(display_width, display_height);
	glutInitWindowPosition(320, 320);
	glutCreateWindow("Chip8 by Tom S");

    glClearColor(0.0, 0.0, 0.0, 0.0);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, display_width, display_height, 0.0); // this is projecting down the Y access

	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutReshapeFunc(reshape_window);

	// possible texture draw
}

void setupInputs()
{
	// OpenGL Inputs - call functions when key is pressed or released
	glutKeyboardFunc(keyboardDown);
	glutKeyboardUpFunc(keyboardUp);
}

// GLUT Callbacks

void display()
{

    // TODO:  Lock emulation cycles to vertical refresh rate (60 FPS)
    timer.start();
    ++instruction_count;

    if (instruction_count == (TARGET_CLOCK_SPEED / SCREEN_REFRESH_RATE)) {
        // update timers
        emu_chip.updateTimers();
        instruction_count = 0;
    }

	// emulate one cycle for the Chip8
	emu_chip.emulateCycle();

	// check the drawFlag to determine if we need to draw anything
	if (emu_chip.drawFlag) {

		// draw routine

		// clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// update the 4 vertice shapes (quads) on screen
		updateQuads();

		// swap buffers 
		glutSwapBuffers();

		// reset the draw flag
		emu_chip.drawFlag = false;
	}

    // check elapsed and wait to slow the emulation down to TARGET_CLOCK_SPEED (540hz)
    timer.end();
    long long elapsed_ns = timer.elapsed();
    while (elapsed_ns < (NANO_SECONDS_PER_HZ / TARGET_CLOCK_SPEED)) {
        timer.end();
        elapsed_ns = timer.elapsed();
    }
}

void reshape_window(GLsizei w, GLsizei h)
{
    // Resize display
    display_width = w;
    display_height = h;

    // update viewport
    glViewport(0, 0, display_width, display_height);
}

void drawPixel(int x, int y)
{
	// draw a 4 sided figure with OpenGL
	glBegin(GL_POLYGON);
		glVertex3f((x * modifier) + 0.0f,               (y * modifier) + 0.0f,              0.0f);    // upper left
		glVertex3f((x * modifier) + 0.0f,               (y * modifier) + modifier + 0.0f,   0.0f);    // lower left
		glVertex3f((x * modifier) + modifier + 0.0f,    (y * modifier) + modifier + 0.0f,   0.0f);    // lower right
		glVertex3f((x * modifier) + modifier + 0.0f,    (y * modifier) + 0.0f,              0.0f);    // upper right
	glEnd();
}

// draw quads -- using global chip8 object
void updateQuads()
{
	// draw on the screen
	for (int y = 0; y < GFX_HEIGHT; ++y) {
		for (int x = 0; x < GFX_WIDTH; ++x) {
			// check if the gfx buffer has a pixel 

			// no pixel
			if (emu_chip.gfx[(y * GFX_WIDTH) + x] == 0) {
				glColor3f(0.0f, 0.0f, 0.0f); // set color black
			}
			// pixel 
			else {
				glColor3f(1.0f, 1.0f, 1.0f);  // set color white
			}

			drawPixel(x, y);
		}
	}
}

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


// main loop
int main_loop(int argc, char** argv) 
{
	// setup render system
	setupGraphics(argc, argv);

    // setup timer
    

	// register input callbacks
	setupInputs();

	// init the chip8 system
	emu_chip.initialize();

	// load the game into memory
	// TODO:  Make this selectable vai a UI
	if (!emu_chip.loadApp(argv[1]))
	{
		emu_chip.debug_simple_msg("Error reading the file provided!");
		return 1;
	}

	glutMainLoop();

	return 0;
}

int main(int argc, char **argv)
{
    return main_loop(argc, argv);
}
