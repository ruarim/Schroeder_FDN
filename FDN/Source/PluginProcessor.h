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
    // TESTING PARAMS - REMOVE IN FINAL
    juce::AudioParameterBool* burstParam;
    float burstGain = 0.0f;
    float burstWidth = 0.10f;
    size_t burstSamples = 0;
    
    /// reverb constants
    static const size_t numDelays = 4;
    static const size_t numOutChannels = 2;
    const float maxDelaySeconds = 2.0f;
    static const int mono = 1;
    static const int stereo = 2;
    const std::array<float, numDelays> allpassDelays = { 0.020346f, 0.024421f, 0.031604f, 0.027333f }; /// - 0.022904f, 0.029291f, 0.013458f, 0.019123f (for higher channel out)
    const std::array<float, numDelays> delayTimes    = { 0.153129f, 0.210389f, 0.127837f, 0.256891f }; /// -  0.174713f, 0.192303f, 0.125000f, 0.219991f (....same)
    
    DelayLine* predelay;
    juce::AudioParameterFloat* predelayParam;
    float predelayTime = 0.0f;
    
    std::array<DelayLine*, numDelays> feedbackDelays; // use juce::dsp instead?
    
    FeedbackMatrix<numDelays> fbMatrix;
    float feedbackDecay = 0.9f; // PARAM
    juce::AudioParameterFloat* feedbackDecayParam;
    
    using Filter            = juce::dsp::IIR::Filter<float>;
    using ArrayCoefficients = juce::dsp::IIR::ArrayCoefficients<float>;
    
    int delayFilterCutoff = 10000; // PARAM
    juce::AudioParameterInt* delayFiltersCutoffParam;
    std::array<Filter, numDelays> delayFilters;
    std::array<SchroederAllpass, numDelays> allpassCombs;
     
    /// master effects
    /// lowshelf
    int lowshelfCutoff   = 200; // PARAM
    juce::AudioParameterInt* lowshelfCutoffParam;
    float lowshelfGain = 0.7f;
    float lowshelfQ    = 0.2f;
        
    /// lowpass
    int lowpassCutoff  = 6000; // PARAM
    juce::AudioParameterInt* lowpassCutoffParam;
    
    enum
    {
        lowshelfIndex,
        lowpassIndex,
    };
    
    using Coefficients = juce::dsp::IIR::Coefficients <float>;
    using StereoFilter = juce::dsp::ProcessorDuplicator<Filter, Coefficients>;
    juce::dsp::ProcessorChain<StereoFilter, StereoFilter> masterEffects;
    
    juce::dsp::DryWetMixer <float> mixer;
    juce::AudioParameterFloat* mixParam;
    float mix = 1.0f;
    
    float testImpulse()
    {
        float signal = (2.0f * (((float)rand() / RAND_MAX) - 0.5f)) * burstGain;
        
        if (burstGain >= 0.0)
            burstGain = burstGain - 1.0 / burstSamples;
        
        return signal;
    }
        
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
    
    void makeUIParams()
    {
        burstParam = new juce::AudioParameterBool("burst", "Burst", false);
        addParameter(burstParam);
        
        lowshelfCutoffParam = new juce::AudioParameterInt("lowshelfCutoff", "Low Shelf Cutoff", 10, 10000, 125);
        addParameter(lowshelfCutoffParam);
        
        lowpassCutoffParam  = new juce::AudioParameterInt("lowpassCutoff", "Low Pass Cutoff", 200, 20000, 7000);
        addParameter(lowpassCutoffParam);
        
        delayFiltersCutoffParam = new juce::AudioParameterInt("delayDampening", "Dampening", 2000, 20000, 10000);
        addParameter(delayFiltersCutoffParam);
        
        feedbackDecayParam = new juce::AudioParameterFloat("feedbackDecay", "Decay", 0.0f, 0.99f, 0.5f);
        addParameter(feedbackDecayParam);
        
        mixParam = new juce::AudioParameterFloat("mix", "Dry/Wet", 0.0f, 1.0f, 1.0f);
        addParameter(mixParam);
        
        predelayParam = new juce::AudioParameterFloat("predelay", "Predelay", 0.0f, maxDelaySeconds/2, 0.0f);
        addParameter(predelayParam);
    }
    
    void getUIParams() // remove and add listener
    {
        lowshelfCutoff = lowshelfCutoffParam->get();
        lowpassCutoff = lowpassCutoffParam->get();
        feedbackDecay = feedbackDecayParam->get();
        delayFilterCutoff = delayFiltersCutoffParam->get();
        mix = mixParam->get();
        predelayTime = predelayParam->get();
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
    
    void scaleDelaysTimes(){} /// multiply the delays by an amount 1 - 5 ?
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FDNAudioProcessor)
};
