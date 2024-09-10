#pragma once
#include <JuceHeader.h>

class crusher {
public:
    void process(juce::dsp::AudioBlock<float>& block);
    void setCrushLevel(float Cl);
private:
    int tempInt;
    float tempFloat;
    float crushLevel = 1;
};
