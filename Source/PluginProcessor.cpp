/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

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
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
parameters (*this, nullptr)
{

    parameters.createAndAddParameter ("switchMode", "Switch mode", "",
                                      NormalisableRange<float> (0.0f, 1.0f, 1.0f), 0.0f,
                                      [](float value)
                                      {
                                          if (value < 0.5f ) return "Exclusive Solo";
                                          else return "ON/OFF mode";
                                      },
                                      nullptr);

    parameters.createAndAddParameter ("channelSize", "Output Channel Size", "channel(s)",
                                      NormalisableRange<float> (1.0f, 32, 1.0f), 2.0f,
                                      [](float value) { return String (value); },
                                      nullptr);

    parameters.createAndAddParameter ("fadeTime", "Fade-Length", "ms",
                                      NormalisableRange<float> (0.0f, 1000.0f, 1.0f), 50.0f,
                                      [](float value) { return String (value); },
                                      nullptr);


    for (int choice = 0; choice < nChoices; ++choice)
    {
        parameters.createAndAddParameter ("choiceState" + String(choice), "Choice " + String::charToString(char ('A' + choice)), "",
                                          NormalisableRange<float> (0.0f, 1.0f, 1.0f), 0.0f,
                                          [](float value)
                                          {
                                              if (value >= 0.5f ) return "ON";
                                              else return "OFF";
                                          },
                                          nullptr);
    }

    parameters.state = ValueTree("ABComparison");


    for (int choice = 0; choice < nChoices; ++choice)
    {
        parameters.addParameterListener ("choiceState" + String(choice), this);
    }
    
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
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
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
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
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
            parameters.replaceState (ValueTree::fromXml (*xmlState));
}

void AbcomparisonAudioProcessor::parameterChanged (const String &parameterID, float newValue)
{

}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AbcomparisonAudioProcessor();
}
