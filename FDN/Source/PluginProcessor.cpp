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
    burstParam = new juce::AudioParameterBool("burst", "Burst", false);
    addParameter(burstParam);
    
    // allocate delays
    for(size_t i = 0; i < numDelays; ++i)
    {
        delays[i] = new DelayLine();
    }
}

FDNAudioProcessor::~FDNAudioProcessor()
{
    for(size_t i = 0; i < numDelays; ++i)
    {
        delete delays[i];
    }
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
        delays[i]->prepare(sampleRate, maxReverbSeconds);
        delays[i]->setReadPosition(delayTimes[i]); // should come from GUI
        
        /// pre feedback matric allpass filters
        allpassCombs[i].prepare(sampleRate, maxReverbSeconds); // may have to instantiate all pass
        allpassCombs[i].setDelay(allpassDelays[i]);
        
        /// delay lowpass filters
        delayFilters[i] .prepare(spec);
        *delayFilters[i].coefficients = Coefficients::makeLowPass(sampleRate, delayFilterCutoff);
    }
    
    masterLowpass.prepare(spec);
    masterLowshelf.prepare(spec);
    
    *masterLowshelf.coefficients = Coefficients::makeLowShelf(sampleRate, lowshelfFreq, lowshelfQ, lowshelfGain);
    *masterLowpass.coefficients = Coefficients::makeLowPass(sampleRate, lowpassFreq);
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
    
    /// output

    for (size_t sample = 0; sample < numSamples; ++sample)
    {
            float signal = buffer.getReadPointer(0)[sample]; // CHANGE
            float out = 0.0f;
            
            // some portion of the signal sent directly out - add dry wet
            //signal *= 0.2; // dry/wet add source receiver delay
            
            std::vector<float> feedbackIn  (numDelays);
            std::vector<float> feedbackOut (numDelays);
            
            /// tap out
            for(size_t i = 0; i < numDelays; ++i)
            {
                float delayed = delays[i]->tapOut();
                out += delayed; /// summed output
                feedbackIn[i] = delayed;
            }
            
            /// apply Schroeder all-pass filters
            for(size_t i = 0; i < numDelays; ++i)
            {
                feedbackIn[i] = allpassCombs[i].processSample(feedbackIn[i]);
            }
            
            /// apply feedback matrix
            feedbackOut = fbMatrix.process(feedbackIn, numDelays, feedbackDampening);
            
            /// tap in
            for(size_t i = 0; i < numDelays; ++i)
            {
                float filterScaling =  (1 / std::sqrt(numDelays)); /// for stability
                //delayFilters[i].snapToZero(); /// see juce_IIRFilter header
                float delayedFiltered = delayFilters[i].processSample(feedbackOut[i]) * filterScaling;
                
                // add feedback and input signal to delay line
                float splitInput = signal / numDelays;
                delays[i]->tapIn(delayedFiltered + splitInput); // avoid instability
                
                // move delays forward one sample
                delays[i]->advance();
            }
            
            /// master filtering
            out = masterLowshelf.processSample(out);
            out = masterLowpass.processSample(out);
        
            for (int channel = 0; channel < totalNumInputChannels; ++channel)
            {
            auto* channelData = buffer.getWritePointer(channel);
            channelData[sample] = out; // replace with call to FDN class.
            }
    }

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
