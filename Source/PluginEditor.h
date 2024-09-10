/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "levelMeter.h"

//==============================================================================
/**
*/
class FaceMelterAudioProcessorEditor  : public juce::AudioProcessorEditor , juce::Timer
{
public:
    FaceMelterAudioProcessorEditor (FaceMelterAudioProcessor&);
    ~FaceMelterAudioProcessorEditor() override;
    void timerCallback() override;
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FaceMelterAudioProcessor& audioProcessor;
    juce::Slider distortionSlider;
    juce::Slider toneSlider;
    juce::Slider volumeSlider;
    juce::Slider crushSlider;

    juce::Label distortionLabel;
    juce::Label toneLabel;
    juce::Label volumeLabel;
    juce::Label crushLabel;
    //juce::TextButton oversampleButton;

    //void buttonClicked(juce::Button* button) override;
    Gui::levelMeter ilevelL, ilevelR, olevel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distortionAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> crushAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FaceMelterAudioProcessorEditor)
};
