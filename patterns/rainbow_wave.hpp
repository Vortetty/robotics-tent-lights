#pragma once
#include "FastLED.h"
#include "../globals.hpp"

#define ANIM_SPEED_MULT 0.5f

namespace rainbow_wave { // Refer to school_colors.hpp for docs
    void setup() {
        FastLED[0].setCorrection(LED_CORRECTION-0x305070);
        FastLED[1].setCorrection(LED_CORRECTION-0x305070);
        FastLED[2].setCorrection(LED_CORRECTION-0x305070);
        FastLED[3].setCorrection(LED_CORRECTION-0x305070);
    }

    void loop() {
        double currentTime = scaledMillis();
        for (int i = 0; i < LEDS_PER_STRIP; i++) {
            leds[i] = CRGB(CHSV(((360*(currentTime/5000)) - ((360/LEDS_PER_STRIP)*i)), 255, 255/2));
        }
        FastLED.show(); 
    }
}