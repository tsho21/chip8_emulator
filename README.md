A basic Chip8 Emulator in C++.

TODO:
 - Fix rendering problems:
      - Graphics move too fast. A simple clock has helped but also slows down player bullets.
	  - Graphics flash off and on.

UPDATE 05/06/2018:
 - Rendering problems most likely due to how the delay timer was being updated.
      - Delay timer should not be updated per cycle
	  - Delay timer should be updated per 60hz cycle

	  - Solution in place but the callback is only called once...
	  - Maybe reset it every time it is fired off? (sounds crappy)

	  - It actually takes 1 timer and 1 delay
		 - 1 timer to update the delay timer and sound timer for the chip8 
		 - 1 delay to slow down the emulation speed (speed you move and inputs are acted upon)