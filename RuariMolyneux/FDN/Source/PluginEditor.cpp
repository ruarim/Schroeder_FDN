/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FDNAudioProcessorEditor::FDNAudioProcessorEditor (FDNAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), revControls(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 300);
    addAndMakeVisible(&revControls);
}

FDNAudioProcessorEditor::~FDNAudioProcessorEditor()
{
}

//==============================================================================
void FDNAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
}

void FDNAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
