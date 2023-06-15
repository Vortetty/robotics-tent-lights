#pragma once
#include "FastLED.h"
#include "../globals.hpp"

namespace usa_flag_colors { // Refer to school_colors.hpp for docs
    void setup() {
        FastLED[0].setCorrection(LED_CORRECTION);
        FastLED[1].setCorrection(LED_CORRECTION);
        FastLED[2].setCorrection(LED_CORRECTION);
        FastLED[3].setCorrection(LED_CORRECTION);
    }

    DEFINE_GRADIENT_PALETTE( flagPaletteGradient ) {
        0,          0xff, 0x00, 0x00,
        255/3.0f,   0xff, 0x00, 0x00,
        255/3.0f,   0xff, 0xff, 0xff,
        255/3.0f*2, 0xff, 0xff, 0xff,
        255/3.0f*2, 0x00, 0x00, 0xff,
        255,        0x00, 0x00, 0xff
    };
    CRGBPalette16 flagPalette = flagPaletteGradient;
    void loop() {
        double currentTime = scaledMillis();
        for (int i = 0; i < LEDS_PER_STRIP; i++) {
            leds[i] = ColorFromPalette(flagPalette, (255*2*(currentTime/5000)) - ((255*2/LEDS_PER_STRIP)*i));
        }
        FastLED.show(); 
    }
}