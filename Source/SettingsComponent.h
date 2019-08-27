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

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class SettingsComponent : public Component
{
public:
    SettingsComponent (AbcomparisonAudioProcessor& p) : processor (p)
    {
        addAndMakeVisible (editor);
        editor.setMultiLine (true);
        editor.setReturnKeyStartsNewLine (true);
        editor.setText (processor.getLabelText());
        editor.onTextChange = [this] () { setLabelText(); };

        addAndMakeVisible (size);
        size.setRange (50, 240, 1);
        size.setValue (processor.getButtonSize());
        size.onValueChange = [this] () { setButtonSize(); };
    }

    ~SettingsComponent()
    {
    }

    void paint (Graphics& g) override
    {
    }

    void setLabelText()
    {
        processor.setLabelText (editor.getText());
    }

    void setButtonSize()
    {
        processor.setButtonSize (size.getValue());
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        bounds.removeFromTop (2);

        auto row = bounds.removeFromBottom (25);
        size.setBounds (row);

        bounds.removeFromBottom (4);

        editor.setBounds (bounds);
    }

private:
    AbcomparisonAudioProcessor& processor;

    TextEditor editor;
    Slider size;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsComponent)
};
