
#include <ESP8266WiFi.h>   // https://links2004.github.io/Arduino/da/de6/class_wi_fi_server.html
#include "GlobalsAndDefines.h"
#include "TVsim.h"

#define  numPixels (sizeof(colors) / sizeof(colors[0]))

TVsim::TVsim()
{
    m_pr = 0;
    m_pg = 0;
    m_pb = 0; // Prev R, G, B
    m_prevStep = efs_none;
}

TVsim::~TVsim()
{
}

void TVsim::init()
{
    randomSeed(analogRead(A0));
    m_pixelNum = random(numPixels); // Begin at random point
}

// to be called in the main 'loop'
uint32_t TVsim::run()
{
    uint32_t delayNextOp = 0;

    switch (m_prevStep)
    {
    case efs_none:
    case efs_initialize:
        // Read next 16-bit (5/6/5) color
        hi = pgm_read_byte(&colors[m_pixelNum * 2]);
        lo = pgm_read_byte(&colors[m_pixelNum * 2 + 1]);
        if (++m_pixelNum >= numPixels) m_pixelNum = 0;

        // Expand to 24-bit (8/8/8)
        r8 = (hi & 0xF8) | (hi >> 5);
        g8 = (hi << 5) | ((lo & 0xE0) >> 3) | ((hi & 0x06) >> 1);
        b8 = (lo << 3) | ((lo & 0x1F) >> 2);
        // Apply gamma correction, further expand to 16/16/16
        nr = (uint8_t)pgm_read_byte(&gamma8[r8]) * 257; // New R/G/B
        ng = (uint8_t)pgm_read_byte(&gamma8[g8]) * 257;
        nb = (uint8_t)pgm_read_byte(&gamma8[b8]) * 257;

        totalTime = random(250, 2500);    // Semi-random pixel-to-pixel time
        fadeTime = random(0, totalTime); // Pixel-to-pixel transition time
        if (random(10) < 3) fadeTime = 0;  // Force scene cut 30% of time
        holdTime = totalTime - fadeTime; // Non-transition time

        startTime = millis();

        // save the next step
        SetNextStep();

        // define time until next call for run
        delayNextOp = 0;

        break;

    case efs_fade:
        elapsed = millis() - startTime;
        if (elapsed >= fadeTime) elapsed = fadeTime;
        if (fadeTime) {
            r = map(elapsed, 0, fadeTime, m_pr, nr); // 16-bit interp
            g = map(elapsed, 0, fadeTime, m_pg, ng);
            b = map(elapsed, 0, fadeTime, m_pb, nb);
        }
        else { // Avoid divide-by-zero in map()
            r = nr;
            g = ng;
            b = nb;
        }
        for (i = 0; i < NUM_LEDS; i++) {
            r8 = r >> 8; // Quantize to 8-bit
            g8 = g >> 8;
            b8 = b >> 8;
            frac = (i << 8) / NUM_LEDS; // LED index scaled to 0-255
            if ((r8 < 255) && ((r & 0xFF) >= frac)) r8++; // Boost some fraction
            if ((g8 < 255) && ((g & 0xFF) >= frac)) g8++; // of LEDs to handle
            if ((b8 < 255) && ((b & 0xFF) >= frac)) b8++; // interp > 8bit
            finalledcolor.r = r8;
            finalledcolor.g = g8;
            finalledcolor.b = b8;
            leds[i] = finalledcolor;
            // strip.setPixelColor(i, r8, g8, b8);
        }
        FastLED.show();
        if (elapsed >= fadeTime)
        {   
            // save the next step
            SetNextStep();

            // define time until next call for run
            delayNextOp = holdTime;
        }
        else
        {
            // define time until next call for run
            delayNextOp = 0;
        }
        break;

    case efs_save_ret_vals:
        m_pr = nr; // Prev RGB = new RGB
        m_pg = ng;
        m_pb = nb;

        // save the next step
        SetNextStep();

        // define time until next call for run
        delayNextOp = 0;

        break;
    }

    // due to the watchdogs (software and hardware) we cannot block
    // the processing to long, hardware watchdog alows max 6 seconds.
    // so instead of waiting we provide the wait time to the callee
    
    return delayNextOp;
    
}

// to be called in the main 'loop'
void TVsim::run_with_active_delay()
{
    delay(run());

}