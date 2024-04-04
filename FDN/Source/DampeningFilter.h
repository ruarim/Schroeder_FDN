/*
  ==============================================================================

    DelayFilterbank.h
    Created: 2 Apr 2024 8:28:35pm
    Author:  Ruari Molyneux

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <cmath>

template<int numDelays>
class DampeningFilter
{
public:
    DampeningFilter() = default;
    ~DampeningFilter() = default;
    
    void prepare(float sampleRate)
    {
        this->sampleRate = sampleRate;
        this->prev = 0.0f;
        //this->alpha = alpha; < 1
    }
        
    float processSample(float sample)
    {
        assert(sampleRate != 0.0f);
        /// difference equation y(n) = g * ((1 - a) x[n] + a * y[n-1]))
        float y = g * ((1 - p) * sample + p * prev);
        prev = y;
        
        return y * delayFilterGain; /// scale the filter output for stablity
    }
    
    void setCoefficients(float t60, float M)
    {
        float exp = (-3 * M) / t60;
        g = std::pow(10, exp);
        p = (std::log(10) / 4) * std::log10(g) * (1-(1/(alpha * alpha)));
    }
    
private:
    float sampleRate = 0.0f;
    
    float g = 0.f;
    float p = 0.0f;
    float alpha = 0.7;
    float prev = 0.0f;
    
    const float delayFilterGain =  (1 / std::sqrt(numDelays));
};
