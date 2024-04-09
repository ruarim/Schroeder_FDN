/*
  ==============================================================================

    SchroederAllpass.h
    Created: 30 Mar 2024 2:03:05pm
    Author:  Ruari Molyneux

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "DelayLine.h"


class SchroederAllpass /// first order allpass comb fitler
{
public:
    SchroederAllpass() = default;
    ~SchroederAllpass() = default;

    void prepare(float sampleRate, float maxDelay)
    {
        xDelay.prepare(sampleRate, maxDelay, 1); // mono delay
        yDelay.prepare(sampleRate, maxDelay, 1);
    }
    
    void setDelay(float delaySeconds)
    {
        xDelay.setReadPosition(delaySeconds);
        yDelay.setReadPosition(delaySeconds);
    }	
    
    float processSample(float sample)
    {
        /// get y/x - M
        float xDelayed = xDelay.tapOut(0); // channel zero
        float yDelayed = yDelay.tapOut(0);
        
        /// difference equation:  y[n] = -g * x[n] + x[n - M] + g * y[n-M]
        float y = -g * sample + xDelayed + g * yDelayed;
        
        /// write to delay line
        xDelay.tapIn(sample, 0);
        yDelay.tapIn(y, 0);
        this->advance();
        return y;
    }

private:
    const float g = 0.7f;
    DelayLine xDelay;
    DelayLine yDelay;
    
    void advance()
    {
        xDelay.advance();
        yDelay.advance();
    }

};
