/*
  ==============================================================================

    Reverb.h
    Created: 9 Apr 2024 3:48:27pm
    Author:  Ruari Molyneux

  ==============================================================================
*/

#pragma once
#include "DelayLine.h"
#include "FeedbackMatrix.h"
#include "SchroederAllpass.h"
#include "DampeningFilter.h"

template<size_t numDelays>
class Reverb
{
public:
    Reverb() = default;
    ~Reverb() = default;
    
    void prepare(float sampleRate, float maxDelaySeconds, const std::array<float, numDelays>& feedbackDelayTimes, const std::array<float, numDelays>& allpassDelayTimes)
    {
        /// init delay times
        this->feedbackDelayTimes = feedbackDelayTimes;
        this->allpassDelayTimes  = allpassDelayTimes;
        
        /// setup predelay
        predelay.prepare(sampleRate, maxDelaySeconds, mono);
        fbMatrix.init(); /// generate the feedback matrix
        
        for(size_t i = 0; i < numDelays; ++i)
        {
            /// set up delays
            feedbackDelays[i].prepare(sampleRate, maxDelaySeconds, mono);
            feedbackDelays[i].setReadPosition(feedbackDelayTimes[i]);
            
            /// pre feedback matric allpass filters
            allpassCombs[i].prepare(sampleRate, maxDelaySeconds);
            allpassCombs[i].setDelay(allpassDelayTimes[i]);
            
            dampeningFilters[i].prepare(sampleRate);
        }
    }
    float process(float sampleIn, float t60, float predelayTime) // pass write pointer
    {
        /// set predelay
        predelay.setReadPosition(predelayTime);
        
        predelay.tapIn(sampleIn, 0);
        sampleIn = predelay.tapOut(0);
        predelay.advance();

        /// output values
        float sampleOut = 0.0f;
        std::array<float, numDelays>   feedbackIn;
        std::array<float, numDelays>   feedbackOut;
            
        /// feedback out
        for(size_t i = 0; i < numDelays; ++i)
        {
            /// get delayed signal
            float delayed = feedbackDelays[i].tapOut(0);
                
            /// sums out
            sampleOut += delayed;
                
            /// apply Schroeder all-pass filters
            feedbackIn[i] = allpassCombs[i].processSample(delayed);
        }

        /// apply feedback matrix
        feedbackOut = fbMatrix.process(feedbackIn, 1.0f);
            
        /// distribute  and scale the input to N number of delays
        std::array<float, numDelays> splitInput = distribute(sampleIn); // take vector instead
            
        /// feedback in
        for(size_t i = 0; i < numDelays; ++i)
        {
            dampeningFilters[i].setCoefficients(t60,  feedbackDelayTimes[i]);
            float delayedFiltered = dampeningFilters[i].processSample(feedbackOut[i]);
                    
            feedbackDelays[i].tapIn(delayedFiltered + splitInput[i], 0);
                
            /// move delays forward one sample
            feedbackDelays[i].advance();
        }
        
        return sampleOut;
            
    }
        

private:
    float sampleRate = 0.0f;
    float mono = 1;
    
    float predelayTime   = 0.0f;
    float t60            = 2.0f;
    
    // predelay
    DelayLine predelay;
        
    // feedback delay lines
    std::array<DelayLine, numDelays> feedbackDelays;
    std::array<float, numDelays> feedbackDelayTimes;
    
    // allpass filter
    std::array<SchroederAllpass, numDelays> allpassCombs;
    std::array<float, numDelays> allpassDelayTimes;
    
    // dampening filter
    std::array<DampeningFilter<numDelays>, numDelays> dampeningFilters;
    
    // feedback matrix
    FeedbackMatrix<numDelays> fbMatrix;
    
    // distribute the input sample over a number of delay lines and allpass filters
    std::array<float, numDelays> distribute(float sample){
        std::array<float, numDelays> distributed;
            
        for(size_t i = 0; i < numDelays; i++)
        {
            /// split and invert sign
            distributed[i] = sample / numDelays;
            if(i > numDelays / 2)  distributed[i] *= -1; // half sign inversion
//            if(i % 2 != 0) distributed[i] *= -1; /// interleaved sign inversion
        }
            
        return distributed;
    }
};

