#include "FastLED.h"    // Control leds
#include "globals.hpp"  // Global variables
#include "patterns.hpp" // Basic setup and led pattern manager

#define DECODE_RC6    // Only enable RC6 protocol for remote control
#include <IRremote.h> // IR sensor lib

// Ensures correct led library version
#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

uint8_t pattern = 0; // Current pattern

void setup() {
    Serial.begin(115200); // Setup serial
    IrReceiver.begin(7);  // Set up the ir reciever on pin 7
  
    basicSetup(); // defined in `patterns.h` runs basic setup, sets up leds to all be in sync, brightness and color temperature are set, then registers all patterns

    // Setup pins for dip switch, 8 is the highest bit, 13 is the lowest bit
    pinMode(8,  INPUT_PULLUP);
    pinMode(9,  INPUT_PULLUP);
    pinMode(10, INPUT_PULLUP);
    pinMode(11, INPUT_PULLUP);
    pinMode(12, INPUT_PULLUP);
    pinMode(13, INPUT_PULLUP);
    
    // Decode the dip switch position, this defines the default pattern, switch is read as a 6 bit binary number bus stored in an 8 bit(since no processor can do single bytes)
    pattern = 
        (digitalRead(13) == LOW) |
        (digitalRead(12) == LOW) << 1 |
        (digitalRead(11) == LOW) << 2 |
        (digitalRead(10) == LOW) << 3 |
        (digitalRead(9)  == LOW) << 4 |
        (digitalRead(8)  == LOW) << 5;

    if (enabledPatterns[pattern]) {   // If selected pattern number is registered then call that pattern's setup function
        patternSetupFuncs[pattern]();
    } else {                          // Sets up the debug program(program 0) since selected pattern is invalid
        pattern = 0;
        patternSetupFuncs[0]();
    }
}

int tmpInt0 = 0;       // Used for speed changing code
double tmpDouble1 = 0; // Used for speed changing code

void loop() {
    if (IrReceiver.decode()) {  // Grab an IR code
        IrReceiver.printIRResultShort(&Serial);

        if ((IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) { // if the recieved code is marked as a repeat then process it, this ensures no double processing since one press on the remote sends one pulse then one repeat
            switch (IrReceiver.decodedIRData.command) { // React to the button press
                case 0x0: // Pattern numbers
                case 0x1: // Pattern numbers
                case 0x2: // Pattern numbers
                case 0x3: // Pattern numbers
                case 0x4: // Pattern numbers
                case 0x5: // Pattern numbers
                case 0x6: // Pattern numbers
                case 0x7: // Pattern numbers
                case 0x8: // Pattern numbers
                case 0x9: // Pattern numbers
                    if (enabledPatterns[IrReceiver.decodedIRData.command]) { // If the pattern selected is registered
                        pattern = IrReceiver.decodedIRData.command; // Set the current pattern to the selected pattern
                        patternSetupFuncs[pattern]();               // Call the pattern's setup function
                    } else {
                        for (int i = 0; i < LEDS_PER_STRIP; i++) leds[i] = CRGB(255, 0, 0); // Flash red for 100ms since selection is invalid
                        FastLED.show();
                        delay(100);
                    }
                    break;

                case 0x14: // Timescale increase
                    timescale = min(LEDS_PER_STRIP, timescale+0.125);         // Increase timescale by 0.125, make the max the number of leds per strip, makes rendering code easy, the timescale will be divided by 10 when calculating elapsed time
                    if (paused) {                                             // Renders the current speed if paused
                        tmpInt0 = 0;                                          // Reset temporary int
                        tmpDouble1 = timescale;                               // Set temporary double to timescale so it can be used for rendering
                        while (tmpInt0 < LEDS_PER_STRIP && tmpDouble1 >= 0) { // Renders most leds
                            leds[tmpInt0] = CRGB(255, 0, 0);                  // Set led to red
                            tmpDouble1--;                                     // Decrement temporary double, when it is less than 1 the loop exists
                            tmpInt0++;                                        // Increment temporary int, the loop will also exit if the value gets higher than the temp int
                        }
                        leds[tmpInt0] = CRGB(tmpDouble1*255, 0, 0);           // Renders the last led, the brightness will be what's left over after the previous leds are rendered
                        FastLED.show();                                       // Show the new led frame
                    }
                    break;
                case 0x15: // Timescale decrease
                    timescale = max(0.125f, timescale-0.125);                 // Increase timescale by 0.125, make the max the number of leds per strip, makes rendering code easy, the timescale will be divided by 10 when calculating elapsed time
                    if (paused) {                                             // Renders the current speed if paused
                        tmpInt0 = 0;                                          // Reset temporary int
                        tmpDouble1 = timescale;                               // Set temporary double to timescale so it can be used for rendering
                        while (tmpInt0 < LEDS_PER_STRIP && tmpDouble1 >= 0) { // Renders most leds
                            leds[tmpInt0] = CRGB(255, 0, 0);                  // Set led to red
                            tmpDouble1--;                                     // Decrement temporary double, when it is less than 1 the loop exists
                            tmpInt0++;                                        // Increment temporary int, the loop will also exit if the value gets higher than the temp int
                        }
                        leds[tmpInt0] = CRGB(tmpDouble1*255, 0, 0);           // Renders the last led, the brightness will be what's left over after the previous leds are rendered
                        FastLED.show();                                       // Show the new led frame
                    }
                    break;

                case 0x16:                         // Play button
                    paused = false;                // Sets pause to false
                    setMillis(lastUnscaledMillis); // Sets the current time
                    break;
                case 0x18:          // Pause button
                    paused = true;  // Sets pause to true
                    scaledMillis(); // Ensure elapsed time is set perfectly to when it was paused
                    break;

                case 0xC:                                                             // Power button
                    off = off ? false : true;                                         // More robust way to toggle off state
                    for (int i = 0; i < LEDS_PER_STRIP; i++) leds[i] = CRGB(0, 0, 0); // Turn off all leds
                    FastLED.show();                                                   // Shows the new led frame
                    resetMillis();                                                    // Reset the millis timer
                    lastUnscaledMillis = 0;                                           // Reset the last unscaled millis counter
                    lastMillis = 0;                                                   // Reset the last millis counter
                    totalMillis = 0;                                                  // Reset the scaled millis counter
                    break;

                default: // If the code is not one that is used then it's useless to this and ignored
                    break;
            }
            IrReceiver.resume(); // Tell the reciever to listen again
        } else {                 // If the code is not a repeat then it's likely truncated since the led rendering code take up most of the cpu power on the single-threaded arduino
                                 // To fix this we make the reciever listen again while running a delay on this code so it can recieve the repeat code
                                 // If someone else is using an ir remote this *could* cause a loop, but it's unlikely anyone will hit the sensor that spot-on
            IrReceiver.resume(); // Tell the reciever to listen again
            delay(100);          // Wait for code to come in, 100ms is normally enough for rc6
        }
        
    }
  
    if (!paused && !off) // If not paused and not off then run the pattern's code
        patternFuncs[pattern]();
}
