/*
  ==============================================================================

    ChannelManager.h
    Created: 2 Apr 2024 8:32:55pm
    Author:  Ruari Molyneux

  ==============================================================================
*/

#pragma once

template<int numDelays, int numOutChannels>
class ChannelManager
{
public:
    ChannelManager() = default;
    ~ChannelManager() = default;
    
    std::array<float, numDelays> stereoDistribute(float left, float right)
    {
        std::array<float, numDelays> distributed;
        for(size_t i = 0; i < numDelays; i++)
        {
            // split left first half right second half
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
        }
        
        return distributed;
    }
    
    void stereoOut(float signal, std::array<float, numOutChannels>& output, size_t count)
    {
        if(count < numDelays / 2) output[0] += signal;
        else output[1] += signal;
    }
};
