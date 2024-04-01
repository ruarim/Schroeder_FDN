/*
  ==============================================================================

    DelayLine.h
    Created: 15 Feb 2024 12:06:06pm
    Author:  Ruari Molyneux

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/// mono delay line
class DelayLine
{
public:
    DelayLine();
    ~DelayLine();
    
    void prepare(float sampleRate, float maxDelaySeconds, int numChannels);
    void setReadPosition(float delayTime);
    float* getWritePointer(int channel);
    
    void tapIn(float sample, int channel); /// add input to delay line at write position
    float tapOut(int channel); /// get output at read position
    void advance();
    
    size_t readPosition = 0;
    size_t writePosition = 0;
    
    float sampleRate = 0.0f;
    juce::AudioSampleBuffer buffer;
    int bufferLength = 0;
    int numChannels = 0;
    
private:
    /// check delay prepared
    void assertPrepare() { assert(sampleRate != 0.0f); }
};
