/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DelayLine.h"
#include "FeedbackMatrix.h"
#include "ParallelProcessor.h"
#include "SchroederAllpass.h"

//==============================================================================
/**
*/
class FDNAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    FDNAudioProcessor();
    ~FDNAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    // TESTING PARAMS - REMOVE
    juce::AudioParameterBool* burstParam;
    float burstGain = 0.0f;
    float burstWidth = 0.10f;
    size_t burstSamples = 0;
    
    /// reverb constants
    static const size_t numDelays = 4; /// 4 left 4 right
    static constexpr float maxReverbSeconds = 2.0f;
    static constexpr const float allpassDelays[numDelays] = { 0.020346f, 0.024421f, 0.031604f, 0.027333f}; /// =  M
    static constexpr float allpassG = 0.6f;
    static constexpr float delayTimes   [numDelays] =  { 0.153129f, 0.210389f, 0.127837f, 0.256891f}; /// =  L + M // feedbackDelays
    // float outGains[numDelays] = { 0.2, 0.4, 0.5, 0.3};  // add array of dampening for more colouration
    
    std::array<DelayLine*, numDelays> delays; // use juce::dsp instead?
    
    FeedbackMatrix fbMatrix;
    float feedbackDampening = 0.5f;
    
    using Filter       = juce::dsp::IIR::Filter<float>;
    using Coefficients = juce::dsp::IIR::ArrayCoefficients<float>;
    int delayFilterCutoff = 10000;
    
    std::array<Filter, numDelays> delayFilters;
    std::array<SchroederAllpass, numDelays> allpassCombs;
    
    // master effects
    Filter masterLowshelf;
    int lowshelfFreq   = 200;
    float lowshelfGain = 0.7f;
    float lowshelfQ    = 0.2;
    
    Filter masterLowpass;
    float lowpassFreq  = 6000.0f;
    
    float testImpulse()
    {
        float signal = (2.0f * (((float)rand() / RAND_MAX) - 0.5f)) * burstGain;
        
        if (burstGain >= 0.0)
            burstGain = burstGain - 1.0 / burstSamples;
        
        return signal;
    }
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FDNAudioProcessor)
};
