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

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "ReverbControls.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
ReverbControls::ReverbControls (FDNAudioProcessor& p)
    : processor(p)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    decaySlider.reset (new juce::Slider ("DecaySlider"));
    addAndMakeVisible (decaySlider.get());
    decaySlider->setRange (0.2, 10, 0.01);
    decaySlider->setSliderStyle (juce::Slider::RotaryVerticalDrag);
    decaySlider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    decaySlider->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    decaySlider->setColour (juce::Slider::trackColourId, juce::Colour (0xff344952));
    decaySlider->setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xff34596a));
    decaySlider->setColour (juce::Slider::textBoxTextColourId, juce::Colours::white);
    decaySlider->addListener (this);

    decaySlider->setBounds (0, 25, 263, 264);

    highpassCutoffRotary.reset (new juce::Slider ("HighpassCutoffRotary"));
    addAndMakeVisible (highpassCutoffRotary.get());
    highpassCutoffRotary->setRange (500, 20, 0.1);
    highpassCutoffRotary->setSliderStyle (juce::Slider::RotaryVerticalDrag);
    highpassCutoffRotary->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    highpassCutoffRotary->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    highpassCutoffRotary->addListener (this);
    highpassCutoffRotary->setSkewFactor (0.9);

    highpassCutoffRotary->setBounds (242, 32, 167, 120);

    lowpassCutoffRotary.reset (new juce::Slider ("LowpassCutoffRotary"));
    addAndMakeVisible (lowpassCutoffRotary.get());
    lowpassCutoffRotary->setRange (500, 20000, 1);
    lowpassCutoffRotary->setSliderStyle (juce::Slider::RotaryVerticalDrag);
    lowpassCutoffRotary->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    lowpassCutoffRotary->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    lowpassCutoffRotary->addListener (this);
    lowpassCutoffRotary->setSkewFactor (0.75);

    lowpassCutoffRotary->setBounds (346, 32, 167, 120);

    predelaySlider.reset (new juce::Slider ("PredelaySlider"));
    addAndMakeVisible (predelaySlider.get());
    predelaySlider->setRange (0, 1, 0.01);
    predelaySlider->setSliderStyle (juce::Slider::LinearHorizontal);
    predelaySlider->setTextBoxStyle (juce::Slider::NoTextBox, false, 80, 20);
    predelaySlider->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    predelaySlider->addListener (this);

    highpassCutoffLabel.reset (new juce::Label ("HighpassCutoffLabel",
                                                TRANS ("Highpass Cutoff")));
    addAndMakeVisible (highpassCutoffLabel.get());
    highpassCutoffLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    highpassCutoffLabel->setJustificationType (juce::Justification::centred);
    highpassCutoffLabel->setEditable (false, false, false);
    highpassCutoffLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    highpassCutoffLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    highpassCutoffLabel->setBounds (251, 152, 150, 24);

    lowpassCutoffLabel.reset (new juce::Label ("LowpassCutoffLabel",
                                               TRANS ("Lowpass Cutoff")));
    addAndMakeVisible (lowpassCutoffLabel.get());
    lowpassCutoffLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    lowpassCutoffLabel->setJustificationType (juce::Justification::centred);
    lowpassCutoffLabel->setEditable (false, false, false);
    lowpassCutoffLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    lowpassCutoffLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    lowpassCutoffLabel->setBounds (357, 152, 150, 24);

    predelayLabel.reset (new juce::Label ("PredelayLabel",
                                          TRANS ("Predelay")));
    addAndMakeVisible (predelayLabel.get());
    predelayLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    predelayLabel->setJustificationType (juce::Justification::centred);
    predelayLabel->setEditable (false, false, false);
    predelayLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    predelayLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    predelayLabel->setBounds (301, 216, 150, 24);

    filtersLabel.reset (new juce::Label ("FilterLabel",
                                         TRANS ("Filter")));
    addAndMakeVisible (filtersLabel.get());
    filtersLabel->setFont (juce::Font (18.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    filtersLabel->setJustificationType (juce::Justification::centred);
    filtersLabel->setEditable (false, false, false);
    filtersLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    filtersLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    filtersLabel->setBounds (301, 5, 150, 24);

    dryWetSlider.reset (new juce::Slider ("DryWetSlider"));
    addAndMakeVisible (dryWetSlider.get());
    dryWetSlider->setRange (0, 1, 0.01);
    dryWetSlider->setSliderStyle (juce::Slider::LinearHorizontal);
    dryWetSlider->setTextBoxStyle (juce::Slider::NoTextBox, false, 80, 20);
    dryWetSlider->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    dryWetSlider->addListener (this);

    dryWetSlider->setBounds (301, 242, 150, 24);

    dryWetLabel.reset (new juce::Label ("DryWetLabel",
                                        TRANS ("Dry/Wet")));
    addAndMakeVisible (dryWetLabel.get());
    dryWetLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    dryWetLabel->setJustificationType (juce::Justification::centred);
    dryWetLabel->setEditable (false, false, false);
    dryWetLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    dryWetLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    dryWetLabel->setBounds (301, 265, 150, 24);

    decayLabel.reset (new juce::Label ("DecayLabel",
                                       TRANS ("Decay - T60dB")));
    addAndMakeVisible (decayLabel.get());
    decayLabel->setFont (juce::Font (18.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    decayLabel->setJustificationType (juce::Justification::centred);
    decayLabel->setEditable (false, false, false);
    decayLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    decayLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    decayLabel->setBounds (55, 5, 150, 24);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

ReverbControls::~ReverbControls()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    decaySlider = nullptr;
    highpassCutoffRotary = nullptr;
    lowpassCutoffRotary = nullptr;
    predelaySlider = nullptr;
    highpassCutoffLabel = nullptr;
    lowpassCutoffLabel = nullptr;
    predelayLabel = nullptr;
    filtersLabel = nullptr;
    dryWetSlider = nullptr;
    dryWetLabel = nullptr;
    decayLabel = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void ReverbControls::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xffffa583));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ReverbControls::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    predelaySlider->setBounds (301, 194, 150, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ReverbControls::sliderValueChanged (juce::Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    float sliderValue = sliderThatWasMoved->getValue();
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == decaySlider.get())
    {
        //[UserSliderCode_decaySlider] -- add your slider handling code here..
        processor.t60 = sliderValue;
        //[/UserSliderCode_decaySlider]
    }
    else if (sliderThatWasMoved == highpassCutoffRotary.get())
    {
        //[UserSliderCode_highpassCutoffRotary] -- add your slider handling code here..
        //[/UserSliderCode_highpassCutoffRotary]
    }
    else if (sliderThatWasMoved == lowpassCutoffRotary.get())
    {
        //[UserSliderCode_lowpassCutoffRotary] -- add your slider handling code here..
        //processor.lowpassCutoff = sliderValue;
        //[/UserSliderCode_lowpassCutoffRotary]
    }
    else if (sliderThatWasMoved == predelaySlider.get())
    {
        //[UserSliderCode_predelaySlider] -- add your slider handling code here..
        processor.predelayTime = sliderValue;
        //[/UserSliderCode_predelaySlider]
    }
    else if (sliderThatWasMoved == dryWetSlider.get())
    {
        //[UserSliderCode_dryWetSlider] -- add your slider handling code here..
        processor.mix = sliderValue;
        //[/UserSliderCode_dryWetSlider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ReverbControls" componentName=""
                 parentClasses="public juce::Component" constructorParams="FDNAudioProcessor&amp; p"
                 variableInitialisers="processor(p)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="0" initialWidth="600"
                 initialHeight="400">
  <BACKGROUND backgroundColour="ffffa583"/>
  <SLIDER name="DecaySlider" id="5c77376dcc19c0dc" memberName="decaySlider"
          virtualName="" explicitFocusOrder="0" pos="0 25 263 264" thumbcol="ffffffff"
          trackcol="ff344952" rotarysliderfill="ff34596a" textboxtext="ffffffff"
          min="0.2" max="10.0" int="0.01" style="RotaryVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="HighpassCutoffRotary" id="503c181d79b83708" memberName="highpassCutoffRotary"
          virtualName="" explicitFocusOrder="0" pos="242 32 167 120" thumbcol="ffffffff"
          min="500.0" max="20.0" int="0.1" style="RotaryVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="0.9"
          needsCallback="1"/>
  <SLIDER name="LowpassCutoffRotary" id="9d7408855718cdc4" memberName="lowpassCutoffRotary"
          virtualName="" explicitFocusOrder="0" pos="346 32 167 120" thumbcol="ffffffff"
          min="500.0" max="20000.0" int="1.0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="0.75" needsCallback="1"/>
  <SLIDER name="PredelaySlider" id="1b9910f3e63b9312" memberName="predelaySlider"
          virtualName="" explicitFocusOrder="0" pos="301 194 150 24" posRelativeW="5c77376dcc19c0dc"
          thumbcol="ffffffff" min="0.0" max="1.0" int="0.01" style="LinearHorizontal"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="HighpassCutoffLabel" id="be51cc5641c1ae02" memberName="highpassCutoffLabel"
         virtualName="" explicitFocusOrder="0" pos="251 152 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Highpass Cutoff" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="LowpassCutoffLabel" id="47a75b3dbb95ef39" memberName="lowpassCutoffLabel"
         virtualName="" explicitFocusOrder="0" pos="357 152 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Lowpass Cutoff" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="PredelayLabel" id="ffd3cec752f34c79" memberName="predelayLabel"
         virtualName="" explicitFocusOrder="0" pos="301 216 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Predelay" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="FilterLabel" id="741f96709b9224b0" memberName="filtersLabel"
         virtualName="" explicitFocusOrder="0" pos="301 5 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Filter" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="18.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <SLIDER name="DryWetSlider" id="8db5e30ca3ef2213" memberName="dryWetSlider"
          virtualName="" explicitFocusOrder="0" pos="301 242 150 24" thumbcol="ffffffff"
          min="0.0" max="1.0" int="0.01" style="LinearHorizontal" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="DryWetLabel" id="99340ac025c3398d" memberName="dryWetLabel"
         virtualName="" explicitFocusOrder="0" pos="301 265 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Dry/Wet" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="DecayLabel" id="5c740aeb820a7d1f" memberName="decayLabel"
         virtualName="" explicitFocusOrder="0" pos="55 5 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Decay - T60dB" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="18.0" kerning="0.0" bold="0" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

