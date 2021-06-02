#include "fastSine.h"

// Main sine lookup table
static const int16_t sineTable[SINE_VAL_COUNT] = {
     0,    490,    980,   1467,   1950,   2429,   2902,   3368,   3826,   4275,
  4713,   5141,   5555,   5956,   6343,   6715,   7071,   7409,   7730,   8032,
  8314,   8577,   8819,   9039,   9238,   9415,   9569,   9700,   9807,   9891,
  9951,   9987,  10000,   9987,   9951,   9891,   9807,   9700,   9569,   9415,
  9238,   9039,   8819,   8577,   8314,   8032,   7730,   7409,   7071,   6715,
  6343,   5956,   5555,   5141,   4713,   4275,   3826,   3368,   2902,   2429,
  1950,   1467,    980,    490,      0,   -490,   -980,  -1467,  -1950,  -2429,
 -2902,  -3368,  -3826,  -4275,  -4713,  -5141,  -5555,  -5956,  -6343,  -6715,
 -7071,  -7409,  -7730,  -8032,  -8314,  -8577,  -8819,  -9039,  -9238,  -9415,
 -9569,  -9700,  -9807,  -9891,  -9951,  -9987, -10000,  -9987,  -9951,  -9891,
 -9807,  -9700,  -9569,  -9415,  -9238,  -9039,  -8819,  -8577,  -8314,  -8032,
 -7730,  -7409,  -7071,  -6715,  -6343,  -5956,  -5555,  -5141,  -4713,  -4275,
 -3826,  -3368,  -2902,  -2429,  -1950,  -1467,   -980,   -490
};


// Fast sin function
int16_t fastSin(uint16_t angle) {

    // Nothing fancy, just return the angle
    return sineTable[angle];
}


// Fast cosine function
int16_t fastCos(uint16_t angle) {

    // Shift the angle 90 degrees forward
    angle += (SINE_VAL_COUNT / 4);

    // Check to make sure that the angle still within bounds. If not, then loop it back around
    if (angle > SINE_VAL_COUNT) {
        angle -= SINE_VAL_COUNT;
    }

    // Return the value in the table
	return sineTable[angle];
}


// Reverse sin lookup (backward)
int16_t sinLookup(int16_t sinValue, ARRAY_SCAN_DIR direction) {

    // First decide which direction to approach the array from
    if (direction == FRONT) {

        // Loop through the values, starting at index 0 and counting to the length of the index
        for (uint8_t counter = 0; counter < SINE_VAL_COUNT; counter++) {

            // Check if the value matches, if so return
            if (sinValue == sineTable[counter]) {
                return counter;
            }
        }
    }
    else {
        // Loop through the values, starting at the length of the index and counting down
        for (uint8_t counter = SINE_VAL_COUNT - 1; counter >= 0; counter--) {

            // Check if the value matches, if so return
            if (sinValue == sineTable[counter]) {
                return counter;
            }
        }
    }

    // If we made it this far, there isn't a value. Return -1
    return -1;
}
