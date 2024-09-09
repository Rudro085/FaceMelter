/*
  ==============================================================================

    wavehsaper.h
    Created: 9 Sep 2024 5:33:04pm
    Author:  LENOVO

  ==============================================================================
*/

#pragma once
#include <cmath>
#include <algorithm>
#include "JuceHeader.h"

class Waveshaper{
public:
    // Constructor
    Waveshaper() {
        V2_prev = Vc1_prev = ic1_prev = V3_prev = Vc5_prev = ic5_prev = 0.0;
        Vc6_prev = ic6_prev = V3_diff_prev = V4_prev = Vc7_prev = ic7_prev = 0.0;
        V5_prev = V6_prev = Vc10_prev = ic10_prev = Vc11_prev = Vc12_prev = ic11_prev = 0.0;
    }
    void prepare(double SampleRate, int SamplePerBlock);
    void setParam(float dist, float tone, float volume, bool osEnabled);
    void process(juce::dsp::AudioBlock<float>& block);

private:

    float solve(float Vin);
    float T,sample_rate,Volume;
    int samplePerBlock;
    // State variables
    float V2_prev, Vc1_prev, ic1_prev, V3_prev, Vc5_prev, ic5_prev;
    float Vc6_prev, ic6_prev, V3_diff_prev, V4_prev, Vc7_prev, ic7_prev;
    float V5_prev, V6_prev, Vc10_prev, ic10_prev, Vc11_prev, Vc12_prev, ic11_prev;

   



    // constant declaration
    float R2 = 1000000.;
    float R3 = 1000.;
    float C1 = 22. / 1000000000.;
    float C2 = 1. / 1000000000.;

    float Rd = 10000.;
    float R4 = 47.;
    float C5 = 2.2 / 1000000.;
    float R5 = 560.;
    float C6 = 4.7 / 1000000.;
    float C4 = 100. / 1000000000000.;
    float R7 = 10000.;
    float C7 = 1. / 1000000.;
    float R8 = 10000.;
    float C8 = 1. / 1000000000.;

    float R11 = 39000.;
    float C10 = 100. / 1000000000.;
    float C9 = 220. / 1000000000000.;
    float R13 = 150000.;

    float Rt = 20000.;
    float C12 = 10. / 1000000.;
    float R15 = 33000.;
    float C11 = 47. / 1000000000.;


    float Vref = 4.5;

    float gc2 = C2 / T;
    float gc1 = T / (2. * C1);
    float gr2 = 1. / R2;
    float gc5 = T / (2. * C5);
    float gc6 = T / (2. * C6);
    float gc4 = C4 / T;
    float grd = 1. / Rd;
    float gr13 = 1. / R13;
    float gc9 = C9 / T;
    float gc10 = T / (2. * C10);
    float gr8 = 1. / R8;
    float gc8 = C8 / T;
    float gc7 = T / (2. * C7);
    float gc11 = T / (2. * C11);
    float gc12 = T / (2. * C12);
};
