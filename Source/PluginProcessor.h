/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "wavehsaper.h"

//==============================================================================
/**
*/
class FaceMelterAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    FaceMelterAudioProcessor();
    ~FaceMelterAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    Waveshaper Ws;
    bool isBypassed = false;
    bool isOsEnabled = true;
    juce::dsp::IIR::Filter<float> filter1,filter2;
    
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    float getLevel(int channel);

private:
    juce::LinearSmoothedValue<float> ilevelL, ilevelR, olevel;
    juce::AudioProcessorValueTreeState apvts;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FaceMelterAudioProcessor)
};
