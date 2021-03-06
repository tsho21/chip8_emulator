#include "Chip8.h"
#include "Timer.h"
#include "GL/glut.h"

int pixel_size = 10;

// window size
int display_width = GFX_WIDTH * pixel_size;
int display_height = GFX_HEIGHT * pixel_size;

// glut functions
void emulate_loop();
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

    glClearColor(0.0f, 0.0f, 0.5f, 0.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, display_width, display_height, 0.0); // this is projecting down the Y access

	glutDisplayFunc(display);
	glutIdleFunc(emulate_loop);
	glutReshapeFunc(reshape_window);
}

void setupInputs()
{
	// OpenGL Inputs - call functions when key is pressed or released
	glutKeyboardFunc(keyboardDown);
	glutKeyboardUpFunc(keyboardUp);
}

// GLUT Callbacks

void emulate_loop() 
{
    // lock clock to 540hz
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
        display();

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

void display()
{
    // draw routine

    // clear the screen
    glClear(GL_COLOR_BUFFER_BIT);

    // update the 4 vertice shapes (quads) on screen
    updateQuads();

    // swap buffers 
    glutSwapBuffers();
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
    glBegin(GL_QUADS);
        glVertex3f((x * pixel_size) + 0.0f, (y * pixel_size) + 0.0f, 0.0f);    // upper left
        glVertex3f((x * pixel_size) + 0.0f, (y * pixel_size) + pixel_size + 0.0f, 0.0f);    // lower left
        glVertex3f((x * pixel_size) + pixel_size + 0.0f, (y * pixel_size) + pixel_size + 0.0f, 0.0f);    // lower right
        glVertex3f((x * pixel_size) + pixel_size + 0.0f, (y * pixel_size) + 0.0f, 0.0f);    // upper right
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
    
    if (key == '1')         { emu_chip.key[0x1] = 1; }
    else if (key == '2')    { emu_chip.key[0x2] = 1; }
    else if (key == '3')    { emu_chip.key[0x3] = 1; }
    else if (key == '4')    { emu_chip.key[0xC] = 1; }

    else if (key == 'q')    { emu_chip.key[0x4] = 1; }
    else if (key == 'w')    { emu_chip.key[0x5] = 1; }
    else if (key == 'e')    { emu_chip.key[0x6] = 1; }
    else if (key == 'r')    { emu_chip.key[0xD] = 1; }

    else if (key == 'a')    { emu_chip.key[0x7] = 1; }
    else if (key == 's')    { emu_chip.key[0x8] = 1; }
    else if (key == 'd')    { emu_chip.key[0x9] = 1; }
    else if (key == 'f')    { emu_chip.key[0xE] = 1; }

    else if (key == 'z')    { emu_chip.key[0xA] = 1; }
    else if (key == 'x')    { emu_chip.key[0x0] = 1; }
    else if (key == 'c')    { emu_chip.key[0xB] = 1; }
    else if (key == 'v')    { emu_chip.key[0xF] = 1; }
}

// unset the keys when the key is released
void keyboardUp(unsigned char key, int x, int y)
{
    if (key == '1') { emu_chip.key[0x1] = 0; }
    else if (key == '2') { emu_chip.key[0x2] = 0; }
    else if (key == '3') { emu_chip.key[0x3] = 0; }
    else if (key == '4') { emu_chip.key[0xC] = 0; }

    else if (key == 'q') { emu_chip.key[0x4] = 0; }
    else if (key == 'w') { emu_chip.key[0x5] = 0; }
    else if (key == 'e') { emu_chip.key[0x6] = 0; }
    else if (key == 'r') { emu_chip.key[0xD] = 0; }

    else if (key == 'a') { emu_chip.key[0x7] = 0; }
    else if (key == 's') { emu_chip.key[0x8] = 0; }
    else if (key == 'd') { emu_chip.key[0x9] = 0; }
    else if (key == 'f') { emu_chip.key[0xE] = 0; }

    else if (key == 'z') { emu_chip.key[0xA] = 0; }
    else if (key == 'x') { emu_chip.key[0x0] = 0; }
    else if (key == 'c') { emu_chip.key[0xB] = 0; }
    else if (key == 'v') { emu_chip.key[0xF] = 0; }
}


// main loop
int main_loop(int argc, char** argv) 
{
	// setup render system
	setupGraphics(argc, argv);
   

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
