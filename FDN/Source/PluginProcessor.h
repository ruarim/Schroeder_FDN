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
    
    // PARAMETERS
    float predelayTime    = 0.0f;
    int delayFilterCutoff = 10000;
    float feedbackDecay   = 0.9f;
    /// lowshelf
    int lowshelfCutoff    = 200;
    float lowshelfGain    = 0.7f;
    float lowshelfQ       = 0.2f;
    /// lowpass
    int lowpassCutoff     = 6000;
    float mix             = 1.0f;

private:    
    /// reverb constants
    static const size_t numDelays = 4;
    static const size_t numOutChannels = 2;
    const float maxDelaySeconds = 2.0f;
    static const int mono = 1;
    static const int stereo = 2;
    const std::array<float, numDelays> allpassDelays = { 0.020346f, 0.024421f, 0.031604f, 0.027333f }; /// - 0.022904f, 0.029291f, 0.013458f, 0.019123f (for higher channel out)
    const std::array<float, numDelays> delayTimes    = { 0.153129f, 0.210389f, 0.127837f, 0.256891f }; /// -  0.174713f, 0.192303f, 0.125000f, 0.219991f (....same)
    
    DelayLine* predelay;
    std::array<DelayLine*, numDelays> feedbackDelays; // use juce::dsp instead?
    FeedbackMatrix<numDelays> fbMatrix;
    
    using Filter            = juce::dsp::IIR::Filter<float>;
    using ArrayCoefficients = juce::dsp::IIR::ArrayCoefficients<float>;
    std::array<Filter, numDelays> delayFilters;
    std::array<SchroederAllpass, numDelays> allpassCombs;
     
    enum
    {
        lowshelfIndex,
        lowpassIndex,
    };
    
    using Coefficients = juce::dsp::IIR::Coefficients <float>;
    using StereoFilter = juce::dsp::ProcessorDuplicator<Filter, Coefficients>;
    juce::dsp::ProcessorChain<StereoFilter, StereoFilter> masterEffects;
    
    juce::dsp::DryWetMixer <float> mixer;
        
    /// distribute input
    std::array<float, numDelays> stereoDistribute(float left, float right)
    {
        std::array<float, numDelays> distributed;
        for(size_t i = 0; i < numDelays; i++)
        {
            if(i < numDelays / 2)
            {
                distributed[i] = left / numDelays;
                if(i % 2 != 0) distributed[i] *= -1; /// sign inversion
            }
            else
            {
                distributed[i] = right / numDelays;
                if(i % 2 != 0) distributed[i] *= -1; /// sign inversion
            }
            
//            /// interleave input
//            if(i % 2 == 0) distributed[i] = left / numDelays;
//            else distributed[i] = right / numDelays;
            
        }
        
        return distributed;
    }
    
    /// collect signals for output
    void stereoOutput(float signal, std::array<float, numOutChannels>& output, size_t i)
    {
        if(i < numDelays / 2) output[0] += signal;
        else output[1] += signal;
    }
        
    void makeFilterCoefficients()
    {
        float sampleRate = getSampleRate();
        for(auto& df : delayFilters) *df.coefficients = ArrayCoefficients::makeLowPass(sampleRate, delayFilterCutoff);
        auto& lowshelf = masterEffects.get<lowshelfIndex>();
        auto& lowpass = masterEffects.get<lowpassIndex>();
        *lowshelf.state = ArrayCoefficients::makeLowShelf(sampleRate, lowshelfCutoff, lowshelfQ, lowshelfGain);
        *lowpass .state = ArrayCoefficients::makeLowPass(sampleRate, lowpassCutoff);
    }
        
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FDNAudioProcessor)
};
