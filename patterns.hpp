#pragma once           // Include guard just in case this is included twice
#include "FastLED.h"   // Control leds
#include "globals.hpp" // Global variables

#include "patterns/test_switches.hpp"  // Import pattern headers
#include "patterns/rainbow_wave.hpp"   // Import pattern headers
#include "patterns/school_colors.hpp"  // Import pattern headers
#include "patterns/twinklefox.hpp"     // Import pattern headers
#include "patterns/red_white_blue.hpp" // Import pattern headers

void(*patternSetupFuncs[POSSIBLE_PATTERNS])() = {}; // Array of pointers to pattern setup functions
void(*patternFuncs[POSSIBLE_PATTERNS])() = {};      // Array of pointers to pattern functions

void registerPattern(int pattern, void(*setupFunc)(), void(*loopFunc)()) { // Registers a pattern, takes a pattern number, a setup function, and a loop function
    patternSetupFuncs[pattern] = setupFunc; // Set the setup function for the pattern
    patternFuncs[pattern] = loopFunc;       // Set the loop function for the pattern
    enabledPatterns[pattern] = true;        // Set the pattern to be enabled
    patternCount++;                         // Increment the pattern count
}

void basicSetup() {
    FastLED.setTemperature(COLOR_TEMP);                                               // Set the color temperature
    FastLED.setBrightness(BRIGHTNESS);                                                // Set the brightness
    FastLED.addLeds<LED_TYPE, 2>(leds, LEDS_PER_STRIP).setCorrection(LED_CORRECTION); // Register strip 1
    FastLED.addLeds<LED_TYPE, 3>(leds, LEDS_PER_STRIP).setCorrection(LED_CORRECTION); // Register strip 2
    FastLED.addLeds<LED_TYPE, 4>(leds, LEDS_PER_STRIP).setCorrection(LED_CORRECTION); // Register strip 3
    FastLED.addLeds<LED_TYPE, 5>(leds, LEDS_PER_STRIP).setCorrection(LED_CORRECTION); // Register strip 4

    registerPattern(0, test_switches::setup,   test_switches::loop);   // Register test_switches,   the debug/test program
    registerPattern(1, rainbow_wave::setup,    rainbow_wave::loop);    // Register rainbow_wave,    simple rainbow pattern
    registerPattern(2, school_colors::setup,   school_colors::loop);   // Register school_colors,   Solid blue with very light blue moving leds
    registerPattern(3, twinklefox::setup,      twinklefox::loop);      // Register twinklefox,      A twinkling blue and white pattern
    registerPattern(4, usa_flag_colors::setup, usa_flag_colors::loop); // Register usa_flag_colors, Red white and blue, made for when the anthem plays
}
