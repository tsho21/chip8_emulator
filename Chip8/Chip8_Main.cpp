#include "Chip8.h"

#include "GL/glut.h"

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

int modifier = 10;

// window size
int display_width = SCREEN_WIDTH * modifier;
int display_height = SCREEN_HEIGHT * modifier;

// glut functions
void display();
void drawPixel(int, int);
void updateQuads();
void reshape_window(GLsizei w, GLsizei h);
void keyboardUp(unsigned char key, int x, int y);
void keyboardDown(unsigned char key, int x, int y);

chip8 testChip8;


// Chip8 Graphics Setup Calls

void setupGraphics(int argc, char **argv)
{
	// Setup OpenGL
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(display_width, display_height);
	glutInitWindowPosition(320, 320);
	glutCreateWindow("Chip8 by Tom S");

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
	// emulate one cycle for the Chip8
	testChip8.emulateCycle();

	// check the drawFlag to determine if we need to draw anything
	if (testChip8.drawFlag) {

		// draw routine

		// clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// update the 4 vertice shapes (quads) on screen
		updateQuads();

		// swap buffers 
		glutSwapBuffers();

		// reset the draw flag
		testChip8.drawFlag = false;
	}
}

void reshape_window(GLsizei h, GLsizei w)
{

}

// set the keys based on the key pressed
void keyboardDown(unsigned char key, int x, int y)
{
	// exit = esc key = 27
	if (key == 27) {
		testChip8.debug_simple_msg("ESC key pressed - exiting program!");
		exit(0);
	}
	
	if (key == KEY_0) {
		testChip8.key[0x0] = 1;
	}
	else if (key == KEY_1) {
		testChip8.key[0x1] = 1;
	}
	else if (key == KEY_2) {
		testChip8.key[0x2] = 1;
	}
	else if (key == KEY_3) {
		testChip8.key[0x3] = 1;
	}
	else if (key == KEY_4) {
		testChip8.key[0x4] = 1;
	}
	else if (key == KEY_5) {
		testChip8.key[0x5] = 1;
	}
	else if (key == KEY_6) {
		testChip8.key[0x6] = 1;
	}
	else if (key == KEY_7) {
		testChip8.key[0x7] = 1;
	}
	else if (key == KEY_8) {
		testChip8.key[0x8] = 1;
	}
	else if (key == KEY_9) {
		testChip8.key[0x9] = 1;
	}
	else if (key == KEY_A) {
		testChip8.key[0xA] = 1;
	}
	else if (key == KEY_B) {
		testChip8.key[0xB] = 1;
	}
	else if (key == KEY_C) {
		testChip8.key[0xC] = 1;
	}
	else if (key == KEY_D) {
		testChip8.key[0xD] = 1;
	}
	else if (key == KEY_E) {
		testChip8.key[0xE] = 1;
	}
	else if (key == KEY_F) {
		testChip8.key[0xF] = 1;
	}
}

// unset the keys when the key is released
void keyboardUp(unsigned char key, int x, int y)
{
	if (key == KEY_0) {
		testChip8.key[0x0] = 0;
	}
	else if (key == KEY_1) {
		testChip8.key[0x1] = 0;
	}
	else if (key == KEY_2) {
		testChip8.key[0x2] = 0;
	}
	else if (key == KEY_3) {
		testChip8.key[0x3] = 0;
	}
	else if (key == KEY_4) {
		testChip8.key[0x4] = 0;
	}
	else if (key == KEY_5) {
		testChip8.key[0x5] = 0;
	}
	else if (key == KEY_6) {
		testChip8.key[0x6] = 0;
	}
	else if (key == KEY_7) {
		testChip8.key[0x7] = 0;
	}
	else if (key == KEY_8) {
		testChip8.key[0x8] = 0;
	}
	else if (key == KEY_9) {
		testChip8.key[0x9] = 0;
	}
	else if (key == KEY_A) {
		testChip8.key[0xA] = 0;
	}
	else if (key == KEY_B) {
		testChip8.key[0xB] = 0;
	}
	else if (key == KEY_C) {
		testChip8.key[0xC] = 0;
	}
	else if (key == KEY_D) {
		testChip8.key[0xD] = 0;
	}
	else if (key == KEY_E) {
		testChip8.key[0xE] = 0;
	}
	else if (key == KEY_F) {
		testChip8.key[0xF] = 0;
	}
}

void drawPixel(int x, int y)
{
	// draw a 4 sided figure with OpenGL
	glBegin(GL_QUADS);
		glVertex3f((x * modifier) + 0.0f, (y * modifier) + 0.0f, 0.0f);   // upper left
		glVertex3f((x * modifier) + 0.0f, (y * modifier) + modifier, 0.0f);  // lower left
		glVertex3f((x * modifier) + modifier, (y * modifier) + modifier, 0.0f); // lower right
		glVertex3f((x * modifier) + modifier, (y * modifier) + 0.0f, 0.0f);  // upper right
	glEnd();
}

// draw quads -- using global chip8 object
void updateQuads()
{
	// draw on the screen
	for (int y = 0; y < SCREEN_HEIGHT; ++y) {
		for (int x = 0; x < SCREEN_WIDTH; ++x) {
			// check if the gfx buffer has a pixel 

			// no pixel
			if (testChip8.gfx[y * SCREEN_WIDTH + x] == 0) {
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

int main(int argc, char **argv)
{
	// setup render system
	setupGraphics(argc, argv);

	// register input callbacks
	setupInputs();

	// init the chip8 system
	testChip8.initialize();

	// load the game into memory
	// TODO:  Make this selectable vai a UI
	if (!testChip8.loadApp(argv[1]))
	{
		testChip8.debug_simple_msg("Error reading the file provided!");
		return 1;
	}

	glutMainLoop();

	return 0;
}
