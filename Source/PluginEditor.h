/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

//==============================================================================
/**
*/
class AbcomparisonAudioProcessorEditor  : public AudioProcessorEditor, public KeyListener, private Timer
{
public:
    AbcomparisonAudioProcessorEditor (AbcomparisonAudioProcessor&, AudioProcessorValueTreeState&);
    ~AbcomparisonAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void updateNumberOfButtons();

    bool keyPressed (const KeyPress &key, Component *originatingComponent) override;
    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AbcomparisonAudioProcessor& processor;
    AudioProcessorValueTreeState& parameters;

    ComboBox cbSwitchMode;
    ComboBox cbChannelSize;
    ComboBox cbNChoices;
    Slider slFadeTime;

    int nChoices = 2;

    ScopedPointer<ComboBoxAttachment> cbSwitchModeAttachment, cbChannelSizeAttachment, cbNChoicesAttachment;
    ScopedPointer<SliderAttachment> slFadeTimeAttachment;

    OwnedArray<TextButton> tbChoice;
    OwnedArray<ButtonAttachment> tbChoiceAttachments;

    FlexBox flexBox;
    
    TooltipWindow toolTipWin;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AbcomparisonAudioProcessorEditor)
};
