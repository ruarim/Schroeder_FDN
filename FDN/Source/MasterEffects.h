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
    MasterEffects() = default;
    ~MasterEffects() = default;
    
    void prepare(juce::dsp::ProcessSpec spec)
    {
        /// set up the processing chain
        chain.prepare(spec);
        
        /// make the filter types
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
    
    /// create filter type aliases - TPT is used to alow modulation of the cutoff with out artifacts
    using Filter = juce::dsp::StateVariableTPTFilter<float>;
    using FilterType = juce::dsp::StateVariableTPTFilterType;
    
    /// define the processing chain
    juce::dsp::ProcessorChain<Filter, Filter> chain;
    
};
