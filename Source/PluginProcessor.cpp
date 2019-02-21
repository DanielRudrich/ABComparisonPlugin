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
AbcomparisonAudioProcessor::AbcomparisonAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::discreteChannels (64), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::discreteChannels (64), true)
                     #endif
                       ),
#endif
parameters (*this, nullptr, "ABComparison", createParameters())
{


    for (int choice = 0; choice < maxNChoices; ++choice)
    {
        parameters.addParameterListener ("choiceState" + String (choice), this);
        choiceStates[choice] = parameters.getRawParameterValue ("choiceState" + String (choice));
    }

    parameters.addParameterListener ("numberOfChoices", this);

    parameters.addParameterListener ("fadeTime", this);
    switchMode = parameters.getRawParameterValue ("switchMode");
    fadeTime = parameters.getRawParameterValue ("fadeTime");
    numberOfChoices = parameters.getRawParameterValue ("numberOfChoices");
}


AbcomparisonAudioProcessor::~AbcomparisonAudioProcessor()
{
}

//==============================================================================
const String AbcomparisonAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AbcomparisonAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AbcomparisonAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AbcomparisonAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AbcomparisonAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AbcomparisonAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AbcomparisonAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AbcomparisonAudioProcessor::setCurrentProgram (int index)
{
}

const String AbcomparisonAudioProcessor::getProgramName (int index)
{
    return {};
}

void AbcomparisonAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void AbcomparisonAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    for (int choice = 0; choice < maxNChoices; ++choice)
    {
        gains[choice].reset (sampleRate, *fadeTime / 1000.0f);
        gains[choice].setValue (*choiceStates[choice] < 0.5f ? 0.0f : 1.0f, true);
    }
}

void AbcomparisonAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AbcomparisonAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return true;
}
#endif

void AbcomparisonAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto nCh = buffer.getNumChannels();
    const int stride = *parameters.getRawParameterValue ("channelSize") + 1;
    auto nSamples = buffer.getNumSamples();
    // choice 0
    if (! gains[0].isSmoothing() && gains[0].getTargetValue() == 0.0f)
    {
        for (int ch = 0; ch < jmin (nCh, stride); ++ch)
            buffer.clear (ch, 0, nSamples);
    }
    else
    {
        const float startGain = gains[0].getNextValue();
        gains[0].skip (nSamples - 2);
        const float endGain = gains[0].getNextValue();

        for (int ch = 0; ch < jmin (nCh, stride); ++ch)
            buffer.applyGainRamp (ch, 0, nSamples, startGain, endGain);
    }

    // remaining choices

    const int nChoices = *numberOfChoices + 2;
    for (int choice = 1; choice < nChoices; ++choice)
    {
        if (gains[choice].isSmoothing() || gains[choice].getTargetValue() != 0.0f)
        {
            const float startGain = gains[choice].getNextValue();
            gains[choice].skip (nSamples - 2);
            const float endGain = gains[choice].getNextValue();

            for (int ch = 0; ch < stride; ++ch)
            {
                const int sourceChannel = choice * stride + ch;
                if (sourceChannel < nCh)
                    buffer.addFromWithRamp (ch, 0, buffer.getReadPointer (sourceChannel), nSamples, startGain, endGain);
            }
        }
    }

    // clear not needed channels
    for (int ch = stride; ch < nCh; ++ch)
        buffer.clear (ch, 0, nSamples);

}

//==============================================================================
bool AbcomparisonAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* AbcomparisonAudioProcessor::createEditor()
{
    return new AbcomparisonAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void AbcomparisonAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void AbcomparisonAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
        {
            parameters.replaceState (ValueTree::fromXml (*xmlState));
            if (parameters.state.hasProperty ("editorWidth") && parameters.state.hasProperty ("editorHeight"))
            {
                editorWidth = parameters.state.getProperty ("editorWidth");
                editorHeight = parameters.state.getProperty ("editorHeight");
                resizeEditorWindow = true;
            }
        }
}

void AbcomparisonAudioProcessor::parameterChanged (const String &parameterID, float newValue)
{
    if (parameterID.startsWith ("choiceState"))
    {
        auto choice = parameterID.substring (11).getIntValue();
        const bool wasOnBefore = gains[choice].getTargetValue() == 1.0f;
        gains[choice].setValue (newValue);
        if (*switchMode < 0.5f && ! mutingOtherChoices) // exclusive solo
        {
            if (wasOnBefore)
                parameters.getParameter ("choiceState" + String (choice))->setValueNotifyingHost (1.0f);
            else
                muteAllOtherChoices (choice);
        }
    }
    else if (parameterID == "fadeTime")
    {
        auto sampleRate = getSampleRate();
        for (int choice = 0; choice < maxNChoices; ++choice)
            gains[choice].reset (sampleRate, *fadeTime / 1000.0f);
    }
    else if (parameterID == "numberOfChoices")
    {
        numberOfChoicesHasChanged = true;
    }
}

void AbcomparisonAudioProcessor::muteAllOtherChoices (const int choiceNotToMute)
{
    ScopedValueSetter<bool> muting (mutingOtherChoices, true);

    for (int i = 0; i < maxNChoices; ++i)
        if (i != choiceNotToMute)
            parameters.getParameter ("choiceState" + String (i))->setValueNotifyingHost (0.0f);
}

AudioProcessorValueTreeState::ParameterLayout AbcomparisonAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back (std::make_unique<AudioProcessorValueTreeState::Parameter> ("switchMode", "Switch mode", "",
                                                                   NormalisableRange<float> (0.0f, 1.0f, 1.0f), 0.0f,
                                                                   [](float value)
                                                                   {
                                                                       if (value < 0.5f ) return "Exclusive Solo";
                                                                       else return "Toggle Mode";
                                                                   },
                                                                                 nullptr));

    params.push_back (std::make_unique<AudioProcessorValueTreeState::Parameter> ("numberOfChoices", "Number of choices", "choices", // has an offset of 2!
                                                                   NormalisableRange<float> (0.0f, maxNChoices - 2.0f, 1.0f), 3.0f,
                                                                   [](float value) { return String (value + 2, 0); },
                                                                                 nullptr));

    params.push_back (std::make_unique<AudioProcessorValueTreeState::Parameter> ("channelSize", "Output Channel Size", "channel (s)", // has an offset of 1!
                                                                   NormalisableRange<float> (0.0f, 31.0f, 1.0f), 1.0f, // default is stereo (2 channels)
                                                                   [](float value) { return String (value + 1, 0); },
                                                                                 nullptr));

    params.push_back (std::make_unique<AudioProcessorValueTreeState::Parameter> ("fadeTime", "Fade-Length", "ms",
                                                                   NormalisableRange<float> (0.0f, 1000.0f, 1.0f), 50.0f,
                                                                   [](float value) { return String (value); },
                                                                                 nullptr));



    for (int choice = 0; choice < maxNChoices; ++choice)
    {
        params.push_back (std::make_unique<AudioProcessorValueTreeState::Parameter> ("choiceState" + String (choice), "Choice " + String::charToString (char ('A' + choice)), "",
                                                                   NormalisableRange<float> (0.0f, 1.0f, 1.0f), 0.0f,
                                                                   [](float value)
                                                                   {
                                                                       if (value >= 0.5f ) return "ON";
                                                                       else return "OFF";
                                                                   },
                                                                   nullptr, true));
    }

    return { params.begin(), params.end() };
}
//==============================================================================




void AbcomparisonAudioProcessor::setEditorSize (int width, int height)
{
    editorWidth = width;
    editorHeight = height;
    DBG ("Editor called setEditorSize: " << width << "x" << height);

    parameters.state.setProperty ("editorWidth", width, nullptr);
    parameters.state.setProperty ("editorHeight", height, nullptr);
}

// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AbcomparisonAudioProcessor();
}
