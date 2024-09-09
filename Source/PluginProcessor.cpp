/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FaceMelterAudioProcessor::FaceMelterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    apvts(*this, nullptr, "PARAMETERS",
        {
            std::make_unique<juce::AudioParameterFloat>("distortion", "Distortion", 0.0f, 1.0f, 0.5f),
            std::make_unique<juce::AudioParameterFloat>("tone", "Tone", 0.0f, 1.0f, 0.5f),
            std::make_unique<juce::AudioParameterFloat>("volume", "Volume", 0.0f, 1.0f, 0.5f)
        }),
    OsProcessor(2, 3, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, false)
#endif
{
}

FaceMelterAudioProcessor::~FaceMelterAudioProcessor()
{
}

//==============================================================================
const juce::String FaceMelterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FaceMelterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FaceMelterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FaceMelterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FaceMelterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FaceMelterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FaceMelterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FaceMelterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FaceMelterAudioProcessor::getProgramName (int index)
{
    return {};
}

void FaceMelterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FaceMelterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    Ws.prepare(sampleRate, samplesPerBlock);
    OsProcessor.initProcessing(samplesPerBlock);
    OsProcessor.reset();
    ilevelL.reset(sampleRate, 0.5);
    ilevelR.reset(sampleRate, 0.5);
    olevel.reset(sampleRate, 0.5);

    ilevelL.setCurrentAndTargetValue(-100.f);
    ilevelR.setCurrentAndTargetValue(-100.f);
    olevel.setCurrentAndTargetValue(-100.f);
}

void FaceMelterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FaceMelterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void FaceMelterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    float distortionValue = *apvts.getRawParameterValue("distortion");
    float toneValue = *apvts.getRawParameterValue("tone");
    float volumeValue = *apvts.getRawParameterValue("volume");
    Ws.setParam(distortionValue, toneValue, volumeValue, isOsEnabled);
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    ilevelL.skip(buffer.getNumSamples());
    ilevelR.skip(buffer.getNumSamples());
    olevel.skip(buffer.getNumSamples());
    {
        float value = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
        if (value < ilevelL.getCurrentValue()) ilevelL.setTargetValue(value);
        else ilevelL.setCurrentAndTargetValue(value);
        value = juce::Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));
        if (value < ilevelR.getCurrentValue()) ilevelR.setTargetValue(value);
        else ilevelR.setCurrentAndTargetValue(value);

    }

    juce::dsp::AudioBlock<float> block(buffer);

    if (isBypassed == false) {
        if (isOsEnabled == false)
            Ws.process(block);
        else {
            juce::dsp::AudioBlock<float> osBlock = OsProcessor.processSamplesUp(block);
            Ws.process(osBlock);
            OsProcessor.processSamplesDown(block);

        }
    }


    for (int channel = 0; channel < block.getNumChannels(); ++channel)
    {
        auto* channelData = block.getChannelPointer(channel);
        for (int sample = 0;sample < block.getNumSamples();++sample) {
            channelData[sample] = block.getSample(channel, sample);
        }
    }

    {
        float value = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
        if (value < olevel.getCurrentValue()) olevel.setTargetValue(value);
        else olevel.setCurrentAndTargetValue(value);
    }
}

//==============================================================================
bool FaceMelterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FaceMelterAudioProcessor::createEditor()
{
    return new FaceMelterAudioProcessorEditor (*this);
}

//==============================================================================
void FaceMelterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FaceMelterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FaceMelterAudioProcessor();
}
float FaceMelterAudioProcessor::getLevel(int channel) {
    if (channel == 0) return ilevelL.getCurrentValue();
    else if (channel == 1) return ilevelR.getCurrentValue();
    else return olevel.getCurrentValue();
}