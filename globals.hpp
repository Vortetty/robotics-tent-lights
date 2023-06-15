#pragma once
#define POSSIBLE_PATTERNS (int)pow(2, 6) /* Number of patterns possible with the dip switch, reduced to 10 when using remote */
#define STRIP_COUNT     4                /* Number of strips */
#define LEDS_PER_STRIP 55                /* Number of leds per strip */
#define LED_TYPE       NEOPIXEL          /* Type of leds */
#define LED_CORRECTION TypicalSMD5050    /* Type of led correction */
#define BRIGHTNESS     128               /* Brightness of leds, 0-255 */
#define COLOR_TEMP     Candle            /* Color temperature of leds */

extern bool enabledPatterns[POSSIBLE_PATTERNS] = {}; /* Array of booleans to keep track of which patterns are enabled */
extern int patternCount = 0;                         /* Number of patterns enabled */
extern double timescale = 10;                        /* Timescale */
extern bool paused = false;                          /* Whether the rendering is paused */
extern bool off = false;                             /* Whether the rendering is off */

double totalMillis = 0;           /* Total time since the program started, scaled, used to keep track of millis */
double lastMillis = 0;            /* Time from start of program to pattern, scaled, used to return when paused and to calculate elapsed time */
long long lastUnscaledMillis = 0; /* Time from start of program to pattern, unscaled, used to pause millis */

extern volatile unsigned long timer0_millis; // Timer 0, the clock used by the arduino to count millis, volatile since it can change ANY time and extern since it's ddfined by the compiler
void resetMillis() {   // Resets millis, allows to reset clock when powered back on
    noInterrupts();    // Disable interrupts so the clock can safely be reset
    timer0_millis = 0; // Reset the clock
    interrupts();      // Re-enable interrupts so it gets incremented again
}
void setMillis(long long millis) { // Sets millis, effectively allows pausing the millis
    noInterrupts();                // Disable interrupts so the clock can safely be set
    timer0_millis = millis;        // Reset the clock
    interrupts();                  // Re-enable interrupts so it gets incremented
}

double scaledMillis() {                                                    // Should be used instead of millis()
    if (!paused) {                                                         // Only update if not paused
        lastUnscaledMillis = millis();                                     // Update lastUnscaledMillis
        totalMillis += lastMillis - (lastUnscaledMillis * (timescale/10)); // Update totalMillis, scales the new millis value then gets the difference and adds it, that way changing timescale doesn't cause glitchiness
        lastMillis = totalMillis;                                          // Update lastMillis
    }
    return lastMillis;
}

extern CRGBArray<LEDS_PER_STRIP> leds = CRGBArray<LEDS_PER_STRIP>(); // Array of led values
