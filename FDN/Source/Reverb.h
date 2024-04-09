/*
  ==============================================================================

    Reverb.h
    Created: 9 Apr 2024 3:27:09pm
    Author:  Ruari Molyneux

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

template<int numDelays>
class Reverb {
public:
    Reverb(){};
    ~Reverb(){};
    
    void prepare();
    void process();
    
private:
    const float gain =  (1 / std::sqrt(numDelays));
    
//    std::array<DelayLine,        numDelays> feedbackDelays;
//    std::array<SchroederAllpass, numDelays> allpassCombs;
//    std::array<DampeningFilter<numDelays>, numDelays> dampeningFilters;
//    FeedbackMatrix<numDelays> fbMatrix;
};
