/*
  ==============================================================================

    DelayLine.h
    Created: 15 Feb 2024 12:06:06pm
    Author:  Ruari Molyneux

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class DelayLine
{
public:
    DelayLine();
    ~DelayLine();
    
    void setReadPosition(float delayTime);
    float* getWritePointer(int channel);
    void prepare(float sampleRate, float maxDelaySeconds);

    void tapIn(float sample); /// add input to delay line at write position
    float tapOut(); /// get output at read position
    
    // process needed for ParallelProcessor
    
    void advance();
    
    size_t readPosition = 0;
    size_t writePosition = 0;
    
    float sampleRate = 0.0f;
    juce::AudioSampleBuffer buffer;
    int bufferLength = 0;
    
private:
    /// check delay prepared
    void assertPrepare() { assert(sampleRate != 0.0f); }
};
