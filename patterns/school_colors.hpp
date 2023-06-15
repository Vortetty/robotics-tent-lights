#pragma once
#include "FastLED.h"
#include "../globals.hpp"

namespace school_colors {
    void setup() { // Setup color correction
        FastLED[0].setCorrection(LED_CORRECTION);
        FastLED[1].setCorrection(LED_CORRECTION);
        FastLED[2].setCorrection(LED_CORRECTION);
        FastLED[3].setCorrection(LED_CORRECTION);
    }


    DEFINE_GRADIENT_PALETTE( schoolColorPaletteGradient ) { // Color palette used for the face
        0,     0x6b, 0xc3, 0xef,
        64,    0x00, 0x21, 0x47,
        255,   0x00, 0x21, 0x47 
    };
    CRGBPalette16 schoolColorPalette = schoolColorPaletteGradient;
    void loop() {
        double currentTime = scaledMillis();       // Get current time
        for (int i = 0; i < LEDS_PER_STRIP; i++) { // Calculate value of each led
            leds[i] = ColorFromPalette(schoolColorPalette, (255*2*(currentTime/5000)) - ((255*2/LEDS_PER_STRIP)*i));
        }
        FastLED.show(); 
    }
}