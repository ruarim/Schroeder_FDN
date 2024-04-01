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
    makeUIParams();
    
    // allocate delays
    for(size_t i = 0; i < numDelays; ++i)
    {
        feedbackDelays[i] = new DelayLine;
    }
    
    predelay = new DelayLine;
}

FDNAudioProcessor::~FDNAudioProcessor()
{
    for(size_t i = 0; i < numDelays; ++i)
    {
        delete feedbackDelays[i];
    }
    
    delete predelay;
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
    burstSamples = (burstWidth * (float)sampleRate);
    
    for(size_t i = 0; i < numDelays; ++i)
    {
        /// set up delays
        feedbackDelays[i]->prepare(sampleRate, maxDelaySeconds, mono);
        feedbackDelays[i]->setReadPosition(delayTimes[i]);
        
        /// pre feedback matric allpass filters
        allpassCombs[i].prepare(sampleRate, maxDelaySeconds);
        allpassCombs[i].setDelay(allpassDelays[i]);
        
        /// delay lowpass filters
        delayFilters[i] .prepare(spec);
    }
    
    predelay->prepare(sampleRate, maxDelaySeconds, stereo);
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
    
    bool burst = burstParam->get();
    if (burst)
    {
        burstParam->setValueNotifyingHost(0);
        burstGain = 1.0f;
    }
    
    getUIParams();
    makeFilterCoefficients();
    
    mixer.setWetMixProportion(mix);
    juce::dsp::AudioBlock <float> block (buffer);
    mixer.pushDrySamples(block);
    
    predelay->setReadPosition(predelayTime); // creates artifacts
        
    /// output
    for (size_t sample = 0; sample < numSamples; ++sample)
    {
        /// output values
        std::array<float, numOutChannels> stereoOut = { 0.0f, 0.0f };
        std::array<float, numDelays>   feedbackIn;
        std::array<float, numDelays>   feedbackOut;
                
        /// feedback out
        for(size_t i = 0; i < numDelays; ++i)
        {
            // get delayed signal
            float delayed = feedbackDelays[i]->tapOut(0);
            
            /// sum  stereos out - CHANGE TO MIXER DESIGN FROM PAPER
            if (i < numDelays / 2) stereoOut[0] += delayed;
            else stereoOut[1] += delayed;
//            stereoOut = stereoOutput(delayed);
            
            feedbackIn[i] = delayed;
        }
        
        /// apply Schroeder all-pass filters - MOVE parralel processor
        for(size_t i = 0; i < numDelays; ++i)
        {
            feedbackIn[i] = allpassCombs[i].processSample(feedbackIn[i]);
        }
            
        /// apply feedback matrix
        feedbackOut = fbMatrix.process(feedbackIn, feedbackDecay);
        
        /// stereo input samples
        std::array<float, stereo> stereoIn = { buffer.getReadPointer(0)[sample], buffer.getReadPointer(1)[sample] };
        
        /// apply pre delay
        for(int channel = 0; channel < stereo; ++channel)
        {
            predelay->tapIn(stereoIn[channel], channel);
            stereoIn[channel] = predelay->tapOut(channel);
        }
        predelay->advance();
        
        /// distribute  and scale the input to N number of delays
        std::array<float, numDelays> splitInput = stereoDistribute(stereoIn[0], stereoIn[1]);
        
        /// feedback in
        for(size_t i = 0; i < numDelays; ++i)
        {
            float delayFilterGain =  (1 / std::sqrt(numDelays)); /// for stability
            delayFilters[i].snapToZero(); /// see juce_IIRFilter header
            
            /// lowpass filter  the feedback output
            float delayedFiltered = delayFilters[i].processSample(feedbackOut[i]) * delayFilterGain;
                
            feedbackDelays[i]->tapIn(delayedFiltered + splitInput[i], 0); // avoid instability
            
            // move delays forward one sample
            feedbackDelays[i]->advance();
        }
        
        /// master filtering - split into 3 bands?
        for(int channel = 0; channel < totalNumOutputChannels; ++channel)
        {
            float out = stereoOut[channel];
            buffer.getWritePointer(channel)[sample] = out;
        }
    }
    
    // apply master effects
    juce::dsp::ProcessContextReplacing<float> context (block);
    masterEffects.process(context);
    mixer.mixWetSamples(block);
}

//==============================================================================
bool FDNAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FDNAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
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
