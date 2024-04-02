/*
  ==============================================================================

    DelayFilterbank.h
    Created: 2 Apr 2024 8:28:35pm
    Author:  Ruari Molyneux

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

template<int numDelays>
class DelayFilters
{
public:
    DelayFilters(){};
    ~DelayFilters(){};
    
    void prepare(juce::dsp::ProcessSpec spec)
    {
        sampleRate = spec.sampleRate;
        lowshelf.prepare(spec);
        lowpass .prepare(spec);
        lowpass .setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    }
        
    float processSample(float sample)
    {
        assert(sampleRate != 0.0f);
        float output = sample;
        //output = lowshelf.processSample(output);
        output = lowpass .processSample(0, output);
        lowshelf.snapToZero();
        lowpass .snapToZero();
        return output * delayFilterGain; // should this be applied to both filter outputs
    }
    
    void setLowpassCutoff(float cutoff)
    {
        lowpass.setCutoffFrequency(cutoff);
    }
    
    void setLowshelfCoefficents(float lowshelfCutoff, float Q, float gain)
    {
        *lowshelf.coefficients = ArrayCoefficients::makeLowShelf(sampleRate, lowshelfCutoff, Q, gain);
    }
    
private:
    float sampleRate = 0.0f;
 

    using Filter            = juce::dsp::IIR::Filter<float>;
    using ArrayCoefficients = juce::dsp::IIR::ArrayCoefficients<float>; // lowshelf
    using TPTFilter         = juce::dsp::StateVariableTPTFilter<float>; // lowpass
    using Coefficients      = juce::dsp::IIR::Coefficients <float>;
    
    enum
    {
        lowshelfIndex,
        lowpassIndex,
    };
    
    Filter lowshelf;
    TPTFilter lowpass;
    
    const float delayFilterGain =  (1 / std::sqrt(numDelays));
};
