/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FaceMelterAudioProcessorEditor::FaceMelterAudioProcessorEditor (FaceMelterAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    setSize(400, 300);
    distortionSlider.setSliderStyle(juce::Slider::Rotary);
    distortionSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(distortionSlider);

    toneSlider.setSliderStyle(juce::Slider::Rotary);
    toneSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(toneSlider);

    volumeSlider.setSliderStyle(juce::Slider::Rotary);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(volumeSlider);

    // Add labels
    distortionLabel.setText("Distortion", juce::dontSendNotification);
    distortionLabel.attachToComponent(&distortionSlider, false);
    addAndMakeVisible(distortionLabel);

    toneLabel.setText("Tone", juce::dontSendNotification);
    toneLabel.attachToComponent(&toneSlider, false);
    addAndMakeVisible(toneLabel);

    volumeLabel.setText("Volume", juce::dontSendNotification);
    volumeLabel.attachToComponent(&volumeSlider, false);
    addAndMakeVisible(volumeLabel);

    addAndMakeVisible(ilevelL);
    addAndMakeVisible(ilevelR);
    addAndMakeVisible(olevel);

    //oversampleButton.setButtonText("Enable Oversampling");
    //oversampleButton.onClick = [this] { buttonClicked(&oversampleButton); };
    //addAndMakeVisible(oversampleButton);

    // Attach sliders to parameters
    distortionAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.getAPVTS(), "distortion", distortionSlider));
    toneAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.getAPVTS(), "tone", toneSlider));
    volumeAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.getAPVTS(), "volume", volumeSlider));

    startTimerHz(24);
}

FaceMelterAudioProcessorEditor::~FaceMelterAudioProcessorEditor()
{
    stopTimer();
}

void FaceMelterAudioProcessorEditor::timerCallback() {
    ilevelL.setLevel(audioProcessor.getLevel(0));
    ilevelL.repaint();
    ilevelR.setLevel(audioProcessor.getLevel(1));
    ilevelR.repaint();
    olevel.setLevel(audioProcessor.getLevel(2));
    olevel.repaint();
}





//==============================================================================
void FaceMelterAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::antiquewhite);

}

void FaceMelterAudioProcessorEditor::resized()
{
    distortionSlider.setBounds(50, 100, 100, 100);
    toneSlider.setBounds(150, 100, 100, 100);
    volumeSlider.setBounds(250, 100, 100, 100);
    ilevelL.setBounds(5, 10, 5, 300 - 20);
    ilevelR.setBounds(12, 10, 5, 300 - 20);
    olevel.setBounds(400 - 10, 10, 5, 300 - 20);
}
