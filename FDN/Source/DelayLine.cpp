/*
  ==============================================================================

    DelayLine.cpp
    Created: 15 Feb 2024 12:06:06pm
    Author:  Ruari Molyneux

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DelayLine.h"

DelayLine::DelayLine(){ };

DelayLine::~DelayLine(){ };

// set up the buffer for the delay line
void DelayLine::prepare(float sampleRate, float maxDelaySeconds, int numChannels){
    this->sampleRate = sampleRate;
    bufferLength = (int)(maxDelaySeconds * sampleRate);
    buffer.setSize(numChannels, bufferLength);
    buffer.clear();
}

// get current read position based on delay time
void DelayLine::setReadPosition(float delayTime){
    assertPrepare();
    readPosition = (int)(writePosition - (delayTime * sampleRate) + bufferLength) % bufferLength;
}

// handle processing the samples here instead of giving write pointer?
void DelayLine::tapIn(float sample, int channel)
{
    assertPrepare();
    buffer.getWritePointer(channel)[writePosition] = sample;
}

float DelayLine::tapOut(int channel)
{
    assertPrepare();
    return buffer.getWritePointer(channel)[readPosition];
}

// call this after writing to the delay line
void DelayLine::advance()
{
    assertPrepare();
    // increment read and write positions and wrap
    if(++readPosition >= bufferLength) readPosition = 0;
    if(++writePosition >= bufferLength) writePosition = 0;
}

