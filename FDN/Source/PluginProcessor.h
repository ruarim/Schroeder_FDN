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
    juce::AudioParameterBool* burstParam;
    
    float burstGain = 0.0f;
    float burstWidth = 0.10;
    size_t burstSamples = 0;
    
    static const size_t numDelays = 4; // 4 left 4 right
    float maxReverbSeconds = 2.0;
    float delayTimes[numDelays] = { 0.153129f, 0.210389f, 0.127837f, 0.256891f};
    float feedbackDampening = 0.125f; // add array of dampening for more colouration
    //float outGains[numDelays] = { 0.2, 0.4, 0.5, 0.3};
    
    std::vector<DelayLine*> delays; // use juce::dsp intead
    FeedbackMatrix fbMatrix;
    
    using Filter       = juce::dsp::IIR::Filter<float>;
    using Coefficients = juce::dsp::IIR::ArrayCoefficients<float>;
    
    int delayFilterCutoff = 8000;
    
    std::array<Filter, numDelays> delayFilters;
    std::array<Filter, numDelays> allpass_combs;
    
    float testImpulse()
    {
        float signal = (2.0f * (((float)rand() / RAND_MAX) - 0.5f)) * burstGain;
        
        if (burstGain >= 0.0)
            burstGain = burstGain - 1.0 / burstSamples;
        
        return signal;
    }
    
    void scaleDelayTimes(float scaleFactor) {
        for (size_t i = 0; i < numDelays; ++i) {
            delayTimes[i] *= scaleFactor;
        }
    }
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FDNAudioProcessor)
};
