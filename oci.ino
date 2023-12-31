
#include <avr/pgmspace.h>
#include <LedControl.h>
#include <Servo.h>


#define LED_ON 1
#define LED_OFF 0

#define RIGHT_EYE 0
#define LEFT_EYE  1
#define PUPIL_Y 4
#define PUPIL_WIDTH 2
#define PUPIL_HEIGHT 2
#define RIGHT_PUPIL_X_ADJUST -2

Servo myservo;
// Initialize LED Controllers

const int devCount = 2;
int brightness = 8;
int pos = 0; 

LedControl lc=LedControl(12, 10, 11, devCount); //MD_MAX72XX M = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES)

uint8_t eyeMatrix[2][8];

// Eye bitmaps array

static const uint8_t PROGMEM // Bitmaps are stored in program memory
blinkImg[][8] = {    // Eye animation frames
    // The EVIL (red) eye, left
    { B00000000,         // Fully open evil/red eye
        B11000000,
        B11111000,
        B01111111,
        B01111111,
        B00111111,
        B00111110,
        B00011100 },
    { B00000000,
        B00000000,
        B11111000,
        B01111111,
        B01111111,
        B00111111,
        B00111110,
        B00011100 },
    { B00000000,
        B00000000,
        B11111000,
        B01111111,
        B01111111,
        B00111111,
        B00111110,
        B00000000 },
    { B00000000,
        B00000000,
        B00000000,
        B01111111,
        B01111111,
        B00111111,
        B00000000,
        B00000000 },
    { B00000000,         // Fully closed evil/red eye
        B00000000,
        B00000000,
        B00000000,
        B01111111,
        B00000000,
        B00000000,
        B00000000 },
    // The EVIL (red) eye, right
    { B00000000,         // Fully open evil/red eye
        B00000011,
        B00011111,
        B11111110,
        B11111110,
        B11111100,
        B01111100,
        B00111000 },
    { B00000000,
        B00000000,
        B00011111,
        B11111110,
        B11111110,
        B11111100,
        B01111100,
        B00111000 },
    { B00000000,
        B00000000,
        B00011111,
        B11111110,
        B11111110,
        B11111100,
        B01111100,
        B00000000 },
    { B00000000,
        B00000000,
        B00000000,
        B11111110,
        B11111110,
        B11111100,
        B00000000,
        B00000000 },
    { B00000000,         // Fully closed evil/red eye
        B00000000,
        B00000000,
        B00000000,
        B11111110,
        B00000000,
        B00000000,
        B00000000 }
};

// Offset to the right eye in the eye array
#define RIGHT_EYE_OFFSET (8 * 5)

// Initialize blink and gaze variables
uint8_t
	blinkIndex[] = { 1, 2, 3, 4, 3, 2, 1 }, // Blink bitmap sequence
    blinkCountdown = 100, // Countdown to next blink (in frames)
    gazeCountdown  =  75, // Countdown to next eye movement
    gazeFrames     =  50; // Duration of eye movement (smaller = faster)
int8_t
    eyeX = 3,   // Current eye position
    newX = 3,   // Next eye position
    dX   = 0;   // Distance from prior to new position

// Draw a pixel
void drawPixel(uint8_t *displaybuffer, int16_t x, int16_t y, uint8_t color) {
	if ((y < 0) || (y >= 8)) return;
	if ((x < 0) || (x >= 8)) return;

	x = 7-x;

	if (color) {
		displaybuffer[y] |= 1 << x;
	} else {
		displaybuffer[y] &= ~(1 << x);
	}
}

// Draw a vertical line
void drawVLine(uint8_t *displaybuffer, int16_t y, int16_t x, int16_t w, uint8_t color) {
	for (int16_t i=x; i<x+w; i++) {
		drawPixel(displaybuffer, i, y, color);
	}
}

// Fill a rectangle with a color
void fillRect(uint8_t *displaybuffer, int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color) {
	for (int16_t i=y; i<y+h; i++) {
		drawVLine(displaybuffer, i, x, w, color);
	}
}

// Clear display buffer
void clear(uint8_t *displaybuffer) {
	for (uint8_t i=0; i<8; i++) {
		displaybuffer[i] = 0;
	}
}

// Copy bitmap from array to display buffer
void drawBitmap(uint8_t *displaybuffer, const uint8_t *source) {
	for (uint8_t i=0; i<8; i++) {
		displaybuffer[i] = pgm_read_byte(&source[i]);
	}
}

// Write display buffer to LED matrix
void writeDisplay(uint8_t *displaybuffer, uint8_t out) {
	for (uint8_t i=0; i<8; i++) {
		lc.setRow(out, i, displaybuffer[i]);
	}
}

// Main setup
void setup() {

	// Seed random number generator from an unused analog input:
	randomSeed(analogRead(A0));
  myservo.attach(5);

	// Initialize LED displays
	for(int address=0;address<devCount;address++) {
		/*The MAX72XX is in power-saving mode on startup*/
		lc.shutdown(address,false);
		/* Set the brightness to a medium values */
		lc.setIntensity(address,brightness);
		/* and clear the display */
		lc.clearDisplay(address);
	}
}

// Main loop
void loop()
{



    uint8_t motion;
    const uint8_t *imageOffset;
    // Draw eyeball in current state of blinkyness (no pupil).
    clear(&eyeMatrix[LEFT_EYE][0]);
    clear(&eyeMatrix[RIGHT_EYE][0]);

    // When counting down to the next blink, show the eye in the fully-
    // open state.  On the last few counts (during the blink), look up
    // the corresponding bitmap index.
    imageOffset = 
            &blinkImg[
            (blinkCountdown < sizeof(blinkIndex)) ? // Currently blinking?
            blinkIndex[blinkCountdown] :            // Yes, look up bitmap #
            0                                       // No, show bitmap 0
            ][0];
    drawBitmap(&eyeMatrix[LEFT_EYE][0], imageOffset);
    imageOffset += RIGHT_EYE_OFFSET;
    drawBitmap(&eyeMatrix[RIGHT_EYE][0], imageOffset);

    // Decrement blink counter.  At end, set random time for next blink.
    blinkCountdown--;
    if (blinkCountdown == 0) {
        blinkCountdown = random(5, 180);
    }


    // Add a pupil (2x2 black square) atop the blinky eyeball bitmap.
    // Periodically, the pupil moves to a new position...
    if(--gazeCountdown <= gazeFrames) {
        // Eyes are in motion - draw pupil at interim position
        fillRect(
                &eyeMatrix[LEFT_EYE][0], 
                newX - (dX * gazeCountdown / gazeFrames),
                PUPIL_Y,
                PUPIL_WIDTH,
                PUPIL_HEIGHT,
                LED_OFF);
        fillRect(
                &eyeMatrix[RIGHT_EYE][0], 
                newX - (dX * gazeCountdown / gazeFrames) + RIGHT_PUPIL_X_ADJUST,
                PUPIL_Y,
                PUPIL_WIDTH,
                PUPIL_HEIGHT,
                LED_OFF);
        if(gazeCountdown == 0) {    // Last frame?
            eyeX = newX; // Yes.  What's new is old, then...
            newX = random(3) + 3;
            dX            = newX - eyeX;             // Horizontal distance to move
            gazeFrames    = random(3, 15);           // Duration of eye movement
            gazeCountdown = random(gazeFrames, 120); // Count to end of next movement
        }
    } else {
        // Not in motion yet -- draw pupil at current static position
        fillRect(
                &eyeMatrix[LEFT_EYE][0],
                eyeX,
                PUPIL_Y,
                PUPIL_WIDTH,
                PUPIL_HEIGHT,
                LED_OFF);
        fillRect(
                &eyeMatrix[RIGHT_EYE][0],
                eyeX + RIGHT_PUPIL_X_ADJUST,
                PUPIL_Y,
                PUPIL_WIDTH,
                PUPIL_HEIGHT,
                LED_OFF);
    }

    // Refresh all of the matrices in one quick pass
    writeDisplay(&eyeMatrix[LEFT_EYE][0], LEFT_EYE);
    writeDisplay(&eyeMatrix[RIGHT_EYE][0], RIGHT_EYE);

    delay(20); // ~50 FPS
  


}