/*
  ==============================================================================

    ParallelProcessor.h
    Created: 29 Mar 2024 12:33:09pm
    Author:  Ruari Molyneux

  ==============================================================================
*/

#pragma once

template <class ProcessorType, size_t numProcessors>
class ParallelProcessor
{
public:
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        for (size_t i = 0; i < numTempBuffers; ++i)
            tempBuffers[i] = juce::dsp::AudioBlock<float> (tempBuffersMemory[i], spec.numChannels, spec.maximumBlockSize);
        
        for (auto& p : processors)
            p.prepare (spec);
    }

    template <class ProcessContext>
    void process (const ProcessContext& context)
    {
        const auto numSamples = context.getInputBlock().getNumSamples();
        
        // The current context might hold smaller blocks than the one that we allocated.
        // This is why we make temporary sub-blocks from the pre-allocated temp buffers
        std::array<juce::dsp::AudioBlock<float>, numTempBuffers> processingTempBlocks;
        
        // Let the first processors process with a non-replacing context that writes to the
        // temp buffers
        for (size_t i = 0; i < numTempBuffers; ++i)
        {
            processingTempBlocks[i] = tempBuffers[i].getSubBlock (0, numSamples);
            processors[i].process (juce::dsp::ProcessContextNonReplacing (context.getInputBlock(), processingTempBlocks[i]));
        }

        // Let the last processor process the context passed in so that it writes to the
        // desired output block
        processors.back().process (context);

        // Accumulate the temporary block data into the output buffer
        for (auto& tempBlock : processingTempBlocks)
            context.getOutputBlock() += tempBlock;
    }

    /** The actual processors */
    std::array<ProcessorType, numProcessors> processors;

private:
    

    static constexpr auto numTempBuffers = numProcessors - 1;
    
    std::array<juce::HeapBlock<char>,        numTempBuffers> tempBuffersMemory;
    std::array<juce::dsp::AudioBlock<float>, numTempBuffers> tempBuffers;
};
