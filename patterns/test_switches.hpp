#pragma once
#include "FastLED.h"
#include "../globals.hpp"

namespace test_switches {
    void setup() { // Setup color correction
        FastLED[0].setCorrection(LED_CORRECTION);
        FastLED[1].setCorrection(LED_CORRECTION);
        FastLED[2].setCorrection(LED_CORRECTION);
        FastLED[3].setCorrection(LED_CORRECTION);
    }

    void loop() {
        int val = // Read dip switch
            (digitalRead(13) == LOW) |
            (digitalRead(12) == LOW) << 1 |
            (digitalRead(11) == LOW) << 2 |
            (digitalRead(10) == LOW) << 3 |
            (digitalRead(9)  == LOW) << 4 |
            (digitalRead(8)  == LOW) << 5;

        leds[val%LEDS_PER_STRIP] = enabledPatterns[val] ? CRGB(0, 255, 0) : CRGB(255, 0, 0); // If pattern is registered then render it green, otherwise red
        FastLED.show();
        leds[val%LEDS_PER_STRIP] = CRGB(0, 0, 0); // Set led back to black for next frame
    }
}