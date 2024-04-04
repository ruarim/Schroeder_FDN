/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 7.0.9

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include <JuceHeader.h>
#include "PluginProcessor.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class ReverbControls  : public juce::Component,
                        public juce::Slider::Listener
{
public:
    //==============================================================================
    ReverbControls (FDNAudioProcessor& p);
    ~ReverbControls() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void sliderValueChanged (juce::Slider* sliderThatWasMoved) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    FDNAudioProcessor& processor;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::Slider> decaySlider;
    std::unique_ptr<juce::Slider> highpassCutoffRotary;
    std::unique_ptr<juce::Slider> lowpassCutoffRotary;
    std::unique_ptr<juce::Slider> predelaySlider;
    std::unique_ptr<juce::Label> highpassCutoffLabel;
    std::unique_ptr<juce::Label> lowpassCutoffLabel;
    std::unique_ptr<juce::Label> predelayLabel;
    std::unique_ptr<juce::Label> filtersLabel;
    std::unique_ptr<juce::Slider> dryWetSlider;
    std::unique_ptr<juce::Label> dryWetLabel;
    std::unique_ptr<juce::Label> decayLabel;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbControls)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

