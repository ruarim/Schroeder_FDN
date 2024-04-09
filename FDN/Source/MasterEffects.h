/*
  ==============================================================================

    MasterEffects.h
    Created: 4 Apr 2024 10:33:55pm
    Author:  Ruari Molyneux

  ==============================================================================
*/

#pragma once

class MasterEffects
{
public:
    void prepare(juce::dsp::ProcessSpec spec)
    {
        chain.prepare(spec);
        chain.get<highpass>().setType(FilterType::highpass);
        chain.get<lowpass>().setType(FilterType::lowpass);
    }
    
    void process(const juce::dsp::ProcessContextReplacing<float>& context)
    {
        chain.process(context);
    }
    
    void setLowpassCutoff(float cutoff)
    {
        chain.get<lowpass>().setCutoffFrequency(cutoff);
    }
    
    void setHighpassCutoff(float cutoff)
    {
        chain.get<highpass>().setCutoffFrequency(cutoff);
    }
    
private:
    enum
    {
        highpass,
        lowpass,
    };
    
    using Filter = juce::dsp::StateVariableTPTFilter<float>;
    using FilterType = juce::dsp::StateVariableTPTFilterType;
    juce::dsp::ProcessorChain<Filter, Filter> chain;
};
