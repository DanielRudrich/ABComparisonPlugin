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
class AbcomparisonAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    AbcomparisonAudioProcessorEditor (AbcomparisonAudioProcessor&, AudioProcessorValueTreeState&);
    ~AbcomparisonAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AbcomparisonAudioProcessor& processor;
    AudioProcessorValueTreeState& parameters;

    ComboBox cbSwitchMode;
    ComboBox cbChannelSize;
    Slider slFadeTime;

    ScopedPointer<ComboBoxAttachment> cbSwitchModeAttachment, cbChannelSizeAttachment;
    ScopedPointer<SliderAttachment> slFadeTimeAttachment;

    OwnedArray<TextButton> tbChoice;
    OwnedArray<ButtonAttachment> tbChoiceAttachments;

    TooltipWindow toolTipWin;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AbcomparisonAudioProcessorEditor)
};
