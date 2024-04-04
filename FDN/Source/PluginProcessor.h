/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DelayLine.h"
#include "FeedbackMatrix.h"
#include "SchroederAllpass.h"
#include "DampeningFilter.h"
#include "ChannelManager.h"

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
    
    // PARAMETERS
    float predelayTime      = 0.0f;
    float t60               = 0.5f;
    /// lowpass
    int   lowpassCutoff     = 6000;
    /// dry wet
    float mix               = 1.0f;

private:    
    /// reverb constants
    static const size_t numDelays = 4;
    static const size_t numOutChannels = 2;
    const  float maxDelaySeconds = 2.0f;
    static const int mono = 1;
    static const int stereo = 2;
    const std::array<float, numDelays> allpassDelays = { 0.020346f, 0.024421f, 0.031604f, 0.027333f }; /// - 0.022904f, 0.029291f, 0.013458f, 0.019123f (for higher channel out)
    const std::array<float, numDelays> delayTimes    = { 0.153129f, 0.210389f, 0.127837f, 0.256891f }; /// -  0.174713f, 0.192303f, 0.125000f, 0.219991f (....same)
    
    DelayLine* predelay;
    std::array<DelayLine*,       numDelays> feedbackDelays;
    std::array<SchroederAllpass, numDelays> allpassCombs;
    std::array<DampeningFilter<numDelays>, numDelays> dampeningFilters;
    FeedbackMatrix<numDelays> fbMatrix;
    juce::dsp::DryWetMixer<float> mixer;
    ChannelManager<numDelays, stereo> channelManager;
    
    /// collect signals for output
    void stereoOutput(float signal, std::array<float, numOutChannels>& output, size_t i)
    {
        if(i < numDelays / 2) output[0] += signal;
        else output[1] += signal;
    }
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FDNAudioProcessor)
};
