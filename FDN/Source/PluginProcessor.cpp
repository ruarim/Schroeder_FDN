/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FDNAudioProcessor::FDNAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

FDNAudioProcessor::~FDNAudioProcessor()
{
}

//==============================================================================
const juce::String FDNAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FDNAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FDNAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FDNAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FDNAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FDNAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FDNAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FDNAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FDNAudioProcessor::getProgramName (int index)
{
    return {};
}

void FDNAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FDNAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // create processing spec
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    
    fbMatrix.init(); /// generate the feedback matrix
    
    for(size_t i = 0; i < numDelays; ++i)
    {
        /// set up delays
        feedbackDelays[i].prepare(sampleRate, maxDelaySeconds, mono);
        feedbackDelays[i].setReadPosition(delayTimes[i]);
        
        /// pre feedback matric allpass filters
        allpassCombs[i].prepare(sampleRate, maxDelaySeconds);
        allpassCombs[i].setDelay(allpassDelays[i]);
        
        dampeningFilters[i].prepare(sampleRate);
    }
    
    predelay.prepare(sampleRate, maxDelaySeconds, stereo);
    masterEffects.prepare(spec);
    mixer.prepare(spec);
    mixer.reset();
}

void FDNAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FDNAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void FDNAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    int numSamples = buffer.getNumSamples();
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    /// set master filter cutoffs
    masterEffects.setLowpassCutoff(lowpassCutoff);
    masterEffects.setHighpassCutoff(highpassCutoff);
    
    /// write dry signal to block
    juce::dsp::AudioBlock <float> block (buffer);
    mixer.setWetMixProportion(mix);
    mixer.pushDrySamples(block);
    
    /// set predelay
    predelay.setReadPosition(predelayTime);
    
    /// run reverb
    for (size_t sample = 0; sample < numSamples; ++sample)
    {
        /// output values
        std::array<float, numOutChannels> stereoOut = { 0.0f, 0.0f };
        std::array<float, numDelays>   feedbackIn;
        std::array<float, numDelays>   feedbackOut;
        
        /// stereo input samples
        std::array<float, stereo> stereoIn = { buffer.getReadPointer(0)[sample], buffer.getReadPointer(1)[sample] };
        
        /// apply pre delay
        predelay.tapIn(stereoIn[0], 0);
        predelay.tapIn(stereoIn[1], 1);
        stereoIn[0] = predelay.tapOut(0);
        stereoIn[1] = predelay.tapOut(1);
        predelay.advance();
        
        
        
                
        /// feedback out
        for(size_t i = 0; i < numDelays; ++i)
        {
            /// get delayed signal
            float delayed = feedbackDelays[i].tapOut(0);
            
            /// apply Schroeder all-pass filters
            feedbackIn[i] = allpassCombs[i].processSample(delayed);
            
            /// sum  stereos out
            channelManager.stereoOut(delayed, stereoOut, i);
        }

        /// apply feedback matrix
        feedbackOut = fbMatrix.process(feedbackIn, normGain);
        
        /// distribute  and scale the input to N number of delays
        std::array<float, numDelays> splitInput = channelManager.stereoDistribute(stereoIn[0], stereoIn[1], normGain); // take vector instead
        
        /// feedback in
        for(size_t i = 0; i < numDelays; ++i)
        {
            dampeningFilters[i].setCoefficients(t60,  delayTimes[i]);
            float delayedFiltered = dampeningFilters[i].processSample(feedbackOut[i], normGain);
                
            feedbackDelays[i].tapIn(delayedFiltered + splitInput[i], 0);
            
            /// move delays forward one sample
            feedbackDelays[i].advance();
        }
        
        /// output
        for(int channel = 0; channel < totalNumOutputChannels; ++channel)
        {
            float out = stereoOut[channel];
            buffer.getWritePointer(channel)[sample] = out;
        }
    }
    
    /// master effects
    masterEffects.process(block);
    
    /// add wet to dry samples
    mixer.mixWetSamples(block);
}

//==============================================================================
bool FDNAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FDNAudioProcessor::createEditor()
{
    return new FDNAudioProcessorEditor(*this);
}

//==============================================================================
void FDNAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FDNAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FDNAudioProcessor();
}
