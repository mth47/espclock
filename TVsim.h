#pragma once

// based on https://learn.adafruit.com/fake-tv-light-for-engineers/arduino-sketch
// by Phillip Burgess

#include "data.h" // Output of Python script

class TVsim
{
public:

    TVsim();

    virtual ~TVsim();

    void init();

    // returns the time in milli seconds when the next run call is needed
    uint32_t run();

    // waits actively
    void run_with_active_delay();

private:

    uint32_t m_pixelNum;
    uint16_t m_pr;
    uint16_t m_pg;
    uint16_t m_pb; // Prev R, G, B


    // these members keep the current fading information
    CRGB finalledcolor;
    uint32_t totalTime, fadeTime, holdTime, startTime, elapsed;
    uint16_t nr, ng, nb, r, g, b, i;
    uint8_t  hi, lo, r8, g8, b8, frac;

    enum eFadingStep
    {
        efs_none = 0,
        efs_initialize,
        efs_fade,
        efs_save_ret_vals,
    } m_prevStep;

        
    // Special behavior for ++eFadingStep
    eFadingStep SetNextStep()
    {
        if (m_prevStep >= TVsim::eFadingStep::efs_save_ret_vals)
            return (m_prevStep = TVsim::eFadingStep::efs_initialize);

        return (m_prevStep = static_cast<TVsim::eFadingStep>(static_cast<int>(m_prevStep) + 1));
    }

};
