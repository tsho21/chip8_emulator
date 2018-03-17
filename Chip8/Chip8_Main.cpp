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
void reshape_window(GLsizei w, GLsizei h);
void keyboardUp(unsigned char key, int x, int y);
void keyboardDown(unsigned char key, int x, int y);

chip8 testChip8;

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
	// OpenGL Inputs
	glutKeyboardFunc(keyboardDown);
	glutKeyboardUpFunc(keyboardUp);
}

void display()
{
	// SDL drawing goes here...
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
	if (!testChip8.loadGame("pong"))
	{
		testChip8.debug_simple_msg("Error reading the file provided!");
	}

	glutMainLoop();

	//// emulation loop
	//for (;;)
	//{
	//	// one cycle
	//	testChip8.emulateCycle();

	//	// if drawFlag set, update the screen
	//	if (testChip8.drawFlag)
	//	{
	//		drawGraphics();
	//	}

	//	// store the key press state (press and release)
	//	testChip8.setKeys();
	//}

	return 0;
}