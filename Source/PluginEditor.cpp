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
    cbSwitchMode.addItem ("Exclusive Solo", 1);
    cbSwitchMode.addItem ("Toggle Mode", 2);
    cbSwitchModeAttachment.reset (new ComboBoxAttachment (parameters, "switchMode", cbSwitchMode));

    addAndMakeVisible (cbNChoices);
    cbNChoices.setJustificationType (Justification::centred);
    for (int i = 2; i <= processor.maxNChoices; ++i)
        cbNChoices.addItem (String (i), i - 1);

    cbNChoicesAttachment.reset (new ComboBoxAttachment (parameters, "numberOfChoices", cbNChoices));

    addAndMakeVisible (cbChannelSize);
    cbChannelSize.setJustificationType (Justification::centred);

    for (int i = 1; i <= 32; ++i)
        cbChannelSize.addItem (String (i) + " ch", i);

    cbChannelSizeAttachment.reset (new ComboBoxAttachment (parameters, "channelSize", cbChannelSize));


    addAndMakeVisible (slFadeTime);
    slFadeTimeAttachment.reset (new SliderAttachment (parameters, "fadeTime", slFadeTime));
    slFadeTime.setTooltip ("Cross-fade time (in ms)");
    slFadeTime.setTextBoxStyle (Slider::TextBoxBelow, false, 80, 20);
    slFadeTime.setTextValueSuffix (" ms");

    addAndMakeVisible (tbEditLabels);
    tbEditLabels.setButtonText ("Labels");
    tbEditLabels.onClick = [this] () { editLabels(); };

    flexBox.flexWrap = FlexBox::Wrap::wrap;
    flexBox.alignContent = FlexBox::AlignContent::flexStart;

    for (int choice = 0; choice < processor.maxNChoices; ++choice)
    {
        auto handle = tbChoice.add (new TextButton);
        addAndMakeVisible (handle);

        tbChoiceAttachments.add (new ButtonAttachment (parameters, "choiceState" + String (choice), *handle));
        handle->setClickingTogglesState (true);
        handle->setColour (TextButton::buttonOnColourId, cols[choice % cols.size()]);
    }
    updateNumberOfButtons();


    // set the size of the GUI so the number of choices (nChoices) will fit in there
    {
        ScopedValueSetter<bool> preventEditorFromCallingProcessor (editorIsResizing, true, false);
        setResizeLimits (460, 300, 1440, 700);
        setSize (jmin (processor.editorWidth.get(), 1440), jmin (processor.editorHeight.get(), 700));
    }

    updateLabelText();
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
    auto titleRow = bounds.removeFromTop (40);

    g.setFont (font);
    g.setColour (Colours::white);
    g.drawFittedText (title, titleRow, Justification::centredTop, 1);

    const float reduceAmount = 0.5f * (titleRow.getWidth() + titleWidth);
    titleRow.removeFromLeft (static_cast<int> (ceil (reduceAmount)));

    String versionString = "v";
    versionString.append (JucePlugin_VersionString, 6);

    g.setFont (12.0f);
    g.drawFittedText (versionString, titleRow, Justification::topLeft, 1);

    auto headlineRow = bounds.removeFromTop (14);
    headlineRow.removeFromLeft (15);

    g.setFont (14.0f);
    g.drawText ("Choices", headlineRow.removeFromLeft (80), Justification::centred, 1);
    g.drawText (CharPointer_UTF8 ("\xc3\xa0"), headlineRow.removeFromLeft (10), Justification::centred, 1);
    g.drawText ("Channels", headlineRow.removeFromLeft (80), Justification::centred, 1);
    headlineRow.removeFromLeft (10);
    g.drawText ("Switch mode", headlineRow.removeFromLeft (120), Justification::centred, 1);
    headlineRow.removeFromLeft (10);
    g.drawText ("FadeTime", headlineRow.removeFromLeft (120), Justification::centred, 1);
}

void AbcomparisonAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop (70);
    bounds.removeFromLeft (15);
    bounds.removeFromRight (15);

    auto settingsArea = bounds.removeFromTop (26);
    cbNChoices.setBounds (settingsArea.removeFromLeft (80));
    settingsArea.removeFromLeft (10);
    cbChannelSize.setBounds (settingsArea.removeFromLeft (80));
    settingsArea.removeFromLeft (10);
    cbSwitchMode.setBounds (settingsArea.removeFromLeft (120));
    settingsArea.removeFromLeft (10);
    slFadeTime.setBounds (settingsArea.removeFromLeft (120).withHeight (45));
    settingsArea.removeFromLeft (10);
    tbEditLabels.setBounds (settingsArea.removeFromLeft (80));

    bounds.removeFromTop (30);

    flexBoxArea = bounds;
    flexBox.performLayout (bounds);

    if (! editorIsResizing) // user is
        processor.setEditorSize (getWidth(), getHeight());
}

bool AbcomparisonAudioProcessorEditor::keyPressed (const KeyPress &key, Component *originatingComponent)
{
    auto choice = key.getKeyCode() - 49;
    if (choice == - 1)
        choice = 9;
    if (choice >= 0 && choice < jmin (nChoices, 10))
        tbChoice.getUnchecked (choice)->triggerClick();
    return true;
}

void AbcomparisonAudioProcessorEditor::updateNumberOfButtons()
{
    flexBox.items.clear();
    nChoices = cbNChoices.getSelectedId() + 1;
    const int size = processor.getButtonSize();

    for (int choice = 0; choice < nChoices; ++choice)
    {
        tbChoice.getUnchecked (choice)->setVisible (true);
        flexBox.items.add (FlexItem (size, size, *tbChoice.getUnchecked (choice)).withMargin ({10, 10, 10, 10}));
    }
    for (int choice = nChoices; choice < processor.maxNChoices; ++choice)
    {
        tbChoice.getUnchecked (choice)->setVisible (false);
    }

    repaint();
}

void AbcomparisonAudioProcessorEditor::timerCallback()
{
    if (cbNChoices.getSelectedId() + 1 != nChoices)
        updateNumberOfButtons();

    if (processor.resizeEditorWindow.get())
    {
        ScopedValueSetter<bool> preventEditorFromCallingProcessor (editorIsResizing, true, false);
        setSize (processor.editorWidth.get(), processor.editorHeight.get());
        processor.resizeEditorWindow = false;
    }

    if (processor.numberOfChoicesHasChanged.get())
    {
        updateNumberOfButtons();
        resized();
        processor.numberOfChoicesHasChanged = false;
    }

    if (processor.updateLabelText.get())
    {
        updateLabelText();
        processor.updateLabelText = false;
    }

    if (processor.updateButtonSize.get())
    {
        updateButtonSize();
        processor.updateButtonSize = false;
    }
}

void AbcomparisonAudioProcessorEditor::editLabels()
{
    auto* settings = new SettingsComponent (processor);
    settings->setSize (300, 250);

    CallOutBox::launchAsynchronously (settings, tbEditLabels.getScreenBounds(), nullptr);
}

void AbcomparisonAudioProcessorEditor::updateLabelText()
{
    auto labels = StringArray::fromLines (processor.getLabelText());

    const int nButtons = tbChoice.size();

    int i = 0;
    for (; i < jmin (labels.size(), nButtons); ++i)
        tbChoice[i]->setButtonText (labels[i]);

    for (; i < nButtons; ++i)
        tbChoice[i]->setButtonText (String (i + 1));
}

void AbcomparisonAudioProcessorEditor::updateButtonSize()
{
    const int size = processor.getButtonSize();

    for (auto& item : flexBox.items)
    {
        item.width = size;
        item.minWidth = size;
        item.height = size;
        item.minHeight = size;
    }

    flexBox.performLayout (flexBoxArea);
}
