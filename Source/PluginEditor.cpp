/*
  ==============================================================================

 ABComparison Plug-in
 Copyright (C) 2018 - Daniel Rudrich

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

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

    addAndMakeVisible (cbNChoices);
    cbNChoices.addSectionHeading("Number of choices");
    for (int i = 2; i <= maxNChoices; ++i)
    {
        cbNChoices.addItem (String(i) + " choices", i - 1);
    }

    cbNChoicesAttachment = new ComboBoxAttachment (parameters, "numberOfChoices", cbNChoices);

    addAndMakeVisible (cbChannelSize);
    cbChannelSize.addSectionHeading("Number of channels per choice");

    for (int i = 1; i <= 32; ++i)
    {
        cbChannelSize.addItem (String (i) + " ch", i);
    }

    cbChannelSizeAttachment = new ComboBoxAttachment (parameters, "channelSize", cbChannelSize);


    addAndMakeVisible (slFadeTime);
    slFadeTimeAttachment = new SliderAttachment (parameters, "fadeTime", slFadeTime);
    slFadeTime.setTooltip ("Cross-fade time (in ms)");
    slFadeTime.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 20);

    flexBox.flexWrap = FlexBox::Wrap::wrap;
    flexBox.alignContent = FlexBox::AlignContent::flexStart;

    for (int choice = 0; choice < maxNChoices; ++choice)
    {
        auto handle = tbChoice.add (new TextButton);
        addAndMakeVisible (handle);

        tbChoiceAttachments.add (new ButtonAttachment (parameters, "choiceState" + String(choice), *handle));
        handle->setClickingTogglesState (true);
        handle->setButtonText (String::charToString(char ('A' + choice)));
        handle->setColour (TextButton::buttonOnColourId, cols[choice % cols.size()]);
    }
    updateNumberOfButtons ();

    // set the size of the GUI so the number of choices (nChoices) will fit in there
    setResizeLimits (460, 300, 1440, 700);
    startTimer (50);
}

AbcomparisonAudioProcessorEditor::~AbcomparisonAudioProcessorEditor()
{
}

//==============================================================================
void AbcomparisonAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    const String title = "ABComparison";

    Font font = Font (30.0f);
    const float titleWidth = font.getStringWidth (title);

    auto bounds = getLocalBounds();
    bounds.removeFromTop (15);
    g.setFont (font);
    g.setColour (Colours::white);
    g.drawFittedText (title, bounds, Justification::centredTop, 1);

    const float reduceAmount = 0.5f * (bounds.getWidth() + titleWidth);
    bounds.removeFromLeft (static_cast<int> (ceil (reduceAmount)));

    String versionString = "v";
    versionString.append (JucePlugin_VersionString, 6);

    g.setFont (12.0f);
    g.drawFittedText (versionString, bounds, Justification::topLeft, 1);
}

void AbcomparisonAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop (80);
    bounds.removeFromLeft (15);
    bounds.removeFromRight (15);

    auto settingsArea = bounds.removeFromTop (30);
    cbNChoices.setBounds (settingsArea.removeFromLeft (100));
    settingsArea.removeFromLeft (10);
    cbChannelSize.setBounds (settingsArea.removeFromLeft (80));
    settingsArea.removeFromLeft (10);
    cbSwitchMode.setBounds (settingsArea.removeFromLeft (120));
    settingsArea.removeFromLeft (10);
    slFadeTime.setBounds (settingsArea.removeFromLeft (160).withHeight (45));
    settingsArea.removeFromLeft (10);

    bounds.removeFromTop (30);

    flexBox.performLayout (bounds);

}

bool AbcomparisonAudioProcessorEditor::keyPressed (const KeyPress &key, Component *originatingComponent)
{
    auto choice = key.getKeyCode() - 49;
    if (choice == - 1)
        choice = 9;
    if (choice >= 0 && choice < jmin (maxNChoices, 10))
        tbChoice.getUnchecked (choice)->triggerClick();
    return true;
}

void AbcomparisonAudioProcessorEditor::updateNumberOfButtons()
{
    flexBox.items.clear();
    nChoices = cbNChoices.getSelectedId() + 1;

    for (int choice = 0; choice < nChoices; ++choice)
    {
        tbChoice.getUnchecked(choice)->setVisible (true);
        flexBox.items.add (FlexItem (120, 120, *tbChoice.getUnchecked(choice)).withMargin ({10, 10, 10, 10}));
    }
    for (int choice = nChoices; choice < maxNChoices; ++choice)
    {
        tbChoice.getUnchecked(choice)->setVisible (false);
    }

    repaint();
}

void AbcomparisonAudioProcessorEditor::timerCallback()
{
    if (cbNChoices.getSelectedId() + 1 != nChoices)
        updateNumberOfButtons();
}
