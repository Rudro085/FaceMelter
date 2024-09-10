


#include "crusher.h"
#include <JuceHeader.h>

void crusher::setCrushLevel(float CL) {
    CL = std::powf(20., 1.03 * CL - 1.) - 0.05;
    crushLevel = 1. +  CL*5000.;
}

void crusher::process(juce::dsp::AudioBlock<float>& block) {
    for (int channel = 0; channel < block.getNumChannels();++channel) {
        for (int sample = 0; sample < block.getNumSamples();++sample) {
            tempInt = block.getSample(channel, sample) * (30000 / crushLevel);
            tempFloat = tempInt / (30000 / crushLevel);
            block.setSample(channel, sample, tempFloat);
        }
    }
}