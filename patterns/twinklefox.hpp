// Written by kriegsman (https://gist.github.com/kriegsman/756ea6dcae8e30845b5a)
// Adapted by Winter/Tæmt modʒiɹæ to work with this system

#pragma once
#include "FastLED.h"
#include "../globals.hpp"

namespace twinklefox
{
    const static int
        TWINKLE_SPEED = 4,
        TWINKLE_DENSITY = 5,
        SECONDS_PER_PALETTE = 30,
        AUTO_SELECT_BACKGROUND_COLOR = 0,
        COOL_LIKE_INCANDESCENT = 0;

    // A mostly blue palette with white accents.
    // "CRGB::Gray" is used as white to keep the brightness more uniform.
    const TProgmemRGBPalette16 BlueWhite_p FL_PROGMEM =
        {CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue,
         CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue,
         CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue,
         CRGB::Blue, CRGB::Gray, CRGB::Gray, CRGB::Gray};

    // A palette of soft snowflakes with the occasional bright one
    const TProgmemRGBPalette16 Snow_p FL_PROGMEM =
        {0x304048, 0x304048, 0x304048, 0x304048,
         0x304048, 0x304048, 0x304048, 0x304048,
         0x304048, 0x304048, 0x304048, 0x304048,
         0x304048, 0x304048, 0x304048, 0xE0F0FF};

    // A cold, icy pale blue palette
#define Ice_Blue1 0x0C1040
#define Ice_Blue2 0x182080
#define Ice_Blue3 0x5080C0
    const TProgmemRGBPalette16 Ice_p FL_PROGMEM =
        {
            Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
            Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
            Ice_Blue2, Ice_Blue2, Ice_Blue2, Ice_Blue2,
            Ice_Blue2, Ice_Blue3, Ice_Blue3, Ice_Blue3};

    const TProgmemRGBPalette16 SchoolColors FL_PROGMEM =
        {
            0x6bc3ef, 0x002147, 0x6bc3ef, 0x002147,
            0x002147, 0x6bc3ef, 0x002147, 0x6bc3ef,
            0x6bc3ef, 0x002147, 0x6bc3ef, 0x002147,
            0x002147, 0x6bc3ef, 0x002147, 0x6bc3ef};

    // Add or remove palette names from this list to control which color
    // palettes are used, and in what order.
    const TProgmemRGBPalette16 *ActivePaletteList[] = {
        &BlueWhite_p,
        &Snow_p,
        &Ice_p,
        &SchoolColors
    };

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

    CRGB gBackgroundColor = CRGB::Black;
    CRGBPalette16 gCurrentPalette, gTargetPalette;

    // This function is like 'triwave8', which produces a
    // symmetrical up-and-down triangle sawtooth waveform, except that this
    // function produces a triangle wave with a faster attack and a slower decay:
    //
    //     / \ 
//    /     \ 
//   /         \ 
//  /             \ 
//

    uint8_t attackDecayWave8(uint8_t i)
    {
        if (i < 86)
        {
            return i * 3;
        }
        else
        {
            i -= 86;
            return 255 - (i + (i / 2));
        }
    }

    // This function takes a pixel, and if its in the 'fading down'
    // part of the cycle, it adjusts the color a little bit like the
    // way that incandescent bulbs fade toward 'red' as they dim.
    void coolLikeIncandescent(CRGB &c, uint8_t phase)
    {
        if (phase < 128)
            return;

        uint8_t cooling = (phase - 128) >> 4;
        c.g = qsub8(c.g, cooling);
        c.b = qsub8(c.b, cooling * 2);
    }

    // Advance to the next color palette in the list (above).
    void chooseNextColorPalette(CRGBPalette16 &pal)
    {
        const uint8_t numberOfPalettes = sizeof(ActivePaletteList) / sizeof(ActivePaletteList[0]);
        static uint8_t whichPalette = -1;
        whichPalette = addmod8(whichPalette, 1, numberOfPalettes);

        pal = *(ActivePaletteList[whichPalette]);
    }

    //  This function takes a time in pseudo-scaledMilliseconds,
    //  figures out brightness = f( time ), and also hue = f( time )
    //  The 'low digits' of the scaledMillisecond time are used as
    //  input to the brightness wave function.
    //  The 'high digits' are used to select a color, so that the color
    //  does not change over the course of the fade-in, fade-out
    //  of one cycle of the brightness wave function.
    //  The 'high digits' are also used to determine whether this pixel
    //  should light at all during this cycle, based on the TWINKLE_DENSITY.
    CRGB computeOneTwinkle(uint32_t ms, uint8_t salt)
    {
        uint16_t ticks = ms >> (8 - TWINKLE_SPEED);
        uint8_t fastcycle8 = ticks;
        uint16_t slowcycle16 = (ticks >> 8) + salt;
        slowcycle16 += sin8(slowcycle16);
        slowcycle16 = (slowcycle16 * 2053) + 1384;
        uint8_t slowcycle8 = (slowcycle16 & 0xFF) + (slowcycle16 >> 8);

        uint8_t bright = 0;
        if (((slowcycle8 & 0x0E) / 2) < TWINKLE_DENSITY)
        {
            bright = attackDecayWave8(fastcycle8);
        }

        uint8_t hue = slowcycle8 - salt;
        CRGB c;
        if (bright > 0)
        {
            c = ColorFromPalette(gCurrentPalette, hue, bright, NOBLEND);
            if (COOL_LIKE_INCANDESCENT == 1)
            {
                coolLikeIncandescent(c, fastcycle8);
            }
        }
        else
        {
            c = CRGB::Black;
        }
        return c;
    }

    //  This function loops over each pixel, calculates the
    //  adjusted 'clock' that this pixel should use, and calls
    //  "CalculateOneTwinkle" on each pixel.  It then displays
    //  either the twinkle color of the background color,
    //  whichever is brighter.
    void drawTwinkles(CRGBSet &L)
    {
        // "PRNG16" is the pseudorandom number generator
        // It MUST be reset to the same starting value each time
        // this function is called, so that the sequence of 'random'
        // numbers that it generates is (paradoxically) stable.
        uint16_t PRNG16 = 11337;

        uint32_t clock32 = scaledMillis();

        // Set up the background color, "bg".
        // if AUTO_SELECT_BACKGROUND_COLOR == 1, and the first two colors of
        // the current palette are identical, then a deeply faded version of
        // that color is used for the background color
        CRGB bg;
        if ((AUTO_SELECT_BACKGROUND_COLOR == 1) &&
            (gCurrentPalette[0] == gCurrentPalette[1]))
        {
            bg = gCurrentPalette[0];
            uint8_t bglight = bg.getAverageLight();
            if (bglight > 64)
            {
                bg.nscale8_video(16); // very bright, so scale to 1/16th
            }
            else if (bglight > 16)
            {
                bg.nscale8_video(64); // not that bright, so scale to 1/4th
            }
            else
            {
                bg.nscale8_video(86); // dim, scale to 1/3rd.
            }
        }
        else
        {
            bg = gBackgroundColor; // just use the explicitly defined background color
        }

        uint8_t backgroundBrightness = bg.getAverageLight();

        for (CRGB &pixel : L)
        {
            PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
            uint16_t myclockoffset16 = PRNG16;         // use that number as clock offset
            PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
            // use that number as clock speed adjustment factor (in 8ths, from 8/8ths to 23/8ths)
            uint8_t myspeedmultiplierQ5_3 = ((((PRNG16 & 0xFF) >> 4) + (PRNG16 & 0x0F)) & 0x0F) + 0x08;
            uint32_t myclock30 = (uint32_t)((clock32 * myspeedmultiplierQ5_3) >> 3) + myclockoffset16;
            uint8_t myunique8 = PRNG16 >> 8; // get 'salt' value for this pixel

            // We now have the adjusted 'clock' for this pixel, now we call
            // the function that computes what color the pixel should be based
            // on the "brightness = f( time )" idea.
            CRGB c = computeOneTwinkle(myclock30, myunique8);

            uint8_t cbright = c.getAverageLight();
            int16_t deltabright = cbright - backgroundBrightness;
            if (deltabright >= 32 || (!bg))
            {
                // If the new pixel is significantly brighter than the background color,
                // use the new color.
                pixel = c;
            }
            else if (deltabright > 0)
            {
                // If the new pixel is just slightly brighter than the background color,
                // mix a blend of the new color and the background color
                pixel = blend(bg, c, deltabright * 8);
            }
            else
            {
                // if the new pixel is not at all brighter than the background color,
                // just use the background color.
                pixel = bg;
            }
        }
    }

    void setup() // Setup led color correction and choose another palette
    {
        FastLED[0].setCorrection(LED_CORRECTION - 0x305070);
        FastLED[1].setCorrection(LED_CORRECTION - 0x305070);
        FastLED[2].setCorrection(LED_CORRECTION - 0x305070);
        FastLED[3].setCorrection(LED_CORRECTION - 0x305070);

        chooseNextColorPalette(gTargetPalette);
    }

    void loop() // Run loop, taken from the link at the file top
    {
        EVERY_N_SECONDS(SECONDS_PER_PALETTE)
        {
            chooseNextColorPalette(gTargetPalette);
        }

        EVERY_N_MILLISECONDS(10)
        {
            nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, 12);
        }

        drawTwinkles(leds);

        FastLED.show();
    }
}
