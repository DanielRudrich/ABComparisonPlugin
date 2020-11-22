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

/*
 ===== TODOs =====
    - simple MIDI interface?
 */

#pragma once
#include "OSCReceiverPlus.h"
#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
*/
class AbcomparisonAudioProcessor :
public AudioProcessor,
private AudioProcessorValueTreeState::Listener,
private OSCReceiver::ListenerWithOSCAddress<OSCReceiver::MessageLoopCallback>
{
    static const juce::Identifier EditorWidth;
    static const juce::Identifier EditorHeight;
    static const juce::Identifier OSCPort;
    static const juce::Identifier OSCEnabled;
    static const juce::Identifier LabelText;
    static const juce::Identifier ButtonSize;
    
public:
    //==============================================================================
    static constexpr int maxNChoices = 32;

    //==============================================================================
    AbcomparisonAudioProcessor();
    ~AbcomparisonAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void parameterChanged (const String &parameterID, float newValue) override;
    void muteAllOtherChoices (const int choiceNotToMute);

    //==============================================================================
    void oscMessageReceived (const OSCMessage&) override;


    // === public flag for editor, signaling to resize window
    Atomic<bool> resizeEditorWindow = false;
    Atomic<bool> updateLabelText = false;
    Atomic<bool> updateButtonSize = false;
    void setEditorSize (int width, int height);
    Atomic<int> editorWidth = 740;
    Atomic<int> editorHeight = 300;
    Atomic<bool> numberOfChoicesHasChanged = false;

    void setLabelText (String labelText);
    const String getLabelText() { return labelText; };

    void setButtonSize (int newSize);
    const int getButtonSize() { return buttonSize.get(); };

    OSCReceiverPlus& getOSCReceiver() noexcept { return oscReceiver; }

private:
    AudioProcessorValueTreeState parameters;
    AudioProcessorValueTreeState::ParameterLayout createParameters();
    LinearSmoothedValue<float> gains[maxNChoices];

    OSCReceiverPlus oscReceiver;

    std::atomic<float>* numberOfChoices;
    std::atomic<float>* switchMode;
    std::atomic<float>* fadeTime;
    std::atomic<float>* choiceStates[maxNChoices];

    bool mutingOtherChoices = false;

    String labelText = "";
    Atomic<int> buttonSize = 120;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AbcomparisonAudioProcessor)
};
