/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AbcomparisonAudioProcessorEditor::AbcomparisonAudioProcessorEditor (AbcomparisonAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), processor (p), parameters (vts)
{
    toolTipWin.setMillisecondsBeforeTipAppears (200);
    toolTipWin.setOpaque (false);

    addKeyListener (this);

    // create a set of colours
    Array<Colour> cols;
    cols.add (Colours::limegreen);
    cols.add (Colours::orange);
    cols.add (Colours::orangered);
    cols.add (Colours::slateblue);
    cols.add (Colours::magenta);

    addAndMakeVisible (cbSwitchMode);
    cbSwitchMode.addSectionHeading("Switch Mode");
    cbSwitchMode.addItem ("Exclusive Solo", 1);
    cbSwitchMode.addItem ("Toggle Mode", 2);
    cbSwitchModeAttachment = new ComboBoxAttachment (parameters, "switchMode", cbSwitchMode);


    addAndMakeVisible (cbChannelSize);
    cbChannelSize.addSectionHeading("Number of channels per choice");

    for (int i = 1; i <= 32; ++i)
    {
        cbChannelSize.addItem (String (i), i);
    }

    cbChannelSizeAttachment = new ComboBoxAttachment (parameters, "channelSize", cbChannelSize);


    addAndMakeVisible (slFadeTime);
    slFadeTimeAttachment = new SliderAttachment (parameters, "fadeTime", slFadeTime);
    slFadeTime.setTooltip ("Cross-fade time (in ms)");

    for (int choice = 0; choice < nChoices; ++choice)
    {
        auto handle = tbChoice.add (new TextButton);
        addAndMakeVisible (handle);
        tbChoiceAttachments.add (new ButtonAttachment (parameters, "choiceState" + String(choice), *handle));
        handle->setClickingTogglesState (true);
        handle->setButtonText (String::charToString(char ('A' + choice)));
        handle->setColour (TextButton::buttonOnColourId, cols[choice % cols.size()]);
    }

    // set the size of the GUI so the number of choices (nChoices) will fit in there
    setSize (680, 300);
}

AbcomparisonAudioProcessorEditor::~AbcomparisonAudioProcessorEditor()
{
}

//==============================================================================
void AbcomparisonAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (30.0f);
    auto bounds = getLocalBounds();
    bounds.removeFromTop (15);
    g.drawFittedText ("ABComparison", bounds, Justification::centredTop, 1);
}

void AbcomparisonAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop (80);
    bounds.removeFromLeft (15);
    bounds.removeFromRight (15);

    auto settingsArea = bounds.removeFromTop (30);
    cbSwitchMode.setBounds (settingsArea.removeFromLeft (150));
    settingsArea.removeFromLeft (20);
    cbChannelSize.setBounds (settingsArea.removeFromLeft (80));
    settingsArea.removeFromLeft (20);
    slFadeTime.setBounds (settingsArea.removeFromLeft (160));
    settingsArea.removeFromLeft (10);

    bounds.removeFromTop (30);

    auto buttonArea = bounds.removeFromTop (120);

    for (int choice = 0; choice < nChoices; ++choice)
    {
        auto handle = tbChoice.getUnchecked (choice);
        handle->setBounds (buttonArea.removeFromLeft (120));
        buttonArea.removeFromLeft (10);
    }
}

bool AbcomparisonAudioProcessorEditor::keyPressed (const KeyPress &key, Component *originatingComponent)
{
    const auto choice = key.getKeyCode() - 49;
    if (choice >= 0 && choice < jmin (nChoices, 10))
        tbChoice.getUnchecked (choice)->triggerClick();
    return true;
}
