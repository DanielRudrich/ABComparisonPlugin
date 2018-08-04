/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

/*
 ===== TODOs =====
    - implement exclusive solo
 */

#pragma once

#define nChoices 5

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
*/
class AbcomparisonAudioProcessor  : public AudioProcessor, private AudioProcessorValueTreeState::Listener
{
public:
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

private:
    AudioProcessorValueTreeState parameters;
    LinearSmoothedValue<float> gains[nChoices];

    float *choiceMode;
    float *fadeTime;
    float *choiceStates[nChoices];


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AbcomparisonAudioProcessor)
};
