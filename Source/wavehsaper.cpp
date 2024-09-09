

#include "wavehsaper.h"



void Waveshaper::setParam(float dist, float tone, float volume, bool osEnabled) {
    if (osEnabled) {
        T = 1. / (8. * sample_rate);
    }
    Rd = std::powf(10., 2. * dist - 2.) * 100000.;
    Rt = 1500. + std::powf(10., 1. * tone - 1) * 100000.;
    Volume = volume;
    //Constants Calculation
    gc2 = C2 / T;
    gc1 = T / (2. * C1);
    gr2 = 1. / R2;
    gc5 = T / (2. * C5);
    gc6 = T / (2. * C6);
    gc4 = C4 / T;
    grd = 1. / Rd;
    gr13 = 1. / R13;
    gc9 = C9 / T;
    gc10 = T / (2. * C10);
    gr8 = 1. / R8;
    gc8 = C8 / T;
    gc7 = T / (2. * C7);
    gc11 = T / (2. * C11);
    gc12 = T / (2. * C12);

}



void Waveshaper::prepare(double SampleRate, int SamplePerBlock) {

    samplePerBlock = SamplePerBlock;
    sample_rate = SampleRate;
    T = 1. / sample_rate;
    setParam(0.5, 0.5, 0.5, false);
    V2_prev = 4.50002888780806;
    Vc1_prev = -4.500028886551527;
    ic1_prev = 2.763001796157738e-11;
    V3_prev = 4.5;
    Vc5_prev = 4.500029018261253;
    ic5_prev = -2.7755998301170726e-09;
    Vc6_prev = 4.500030442143965;
    ic6_prev = -2.8026668132752496e-09;

    V3_diff_prev = 0.;
    V4_prev = 0.;
    Vc7_prev = 0.;
    ic7_prev = 0.;


    V5_prev = 0.;
    V6_prev = 0.;
    Vc10_prev = 0.;
    ic10_prev = 0.;

    Vc11_prev = 0.;
    Vc12_prev = 0.;
    ic11_prev = 0.;
}


// The solve method
float Waveshaper::solve(float Vin) {
    // Input filter
    double V2 = (R3 * V2_prev * gc1 * gc2 * gr2 + R3 * V2_prev * gc2 + V2_prev * gc1 * gc2 - Vc1_prev + Vin +
        Vref * gc1 * gr2 - gc1 * ic1_prev) /
        (R3 * gc1 * gc2 * gr2 + R3 * gc2 + gc1 * gc2 + gc1 * gr2 + 1.0);

    // Memory element update
    Vc1_prev += gc1 * (gc2 * (V2 - V2_prev) + gr2 * (R3 * gc2 * (V2 - V2_prev) + V2 - Vref) + ic1_prev);
    ic1_prev = gc2 * (V2 - V2_prev) + gr2 * (R3 * gc2 * (V2 - V2_prev) + V2 - Vref);
    V2_prev = V2;

    // Op-amp section
    double V3 = (R4 * R5 * V2 * gc4 + R4 * R5 * V2 * grd - R4 * R5 * V2_prev * gc4 + R4 * R5 * V3_prev * gc4 +
        R4 * V2 * gc4 * gc6 + R4 * V2 * gc6 * grd + R4 * V2 - R4 * V2_prev * gc4 * gc6 + R4 * V3_prev * gc4 * gc6 -
        R4 * Vc6_prev - R4 * gc6 * ic6_prev + R5 * V2 * gc5 * gc4 + R5 * V2 * gc5 * grd + R5 * V2 -
        R5 * V2_prev * gc5 * gc4 + R5 * V3_prev * gc5 * gc4 - R5 * Vc5_prev - R5 * gc5 * ic5_prev +
        V2 * gc5 * gc4 * gc6 + V2 * gc5 * gc6 * grd + V2 * gc5 + V2 * gc6 - V2_prev * gc5 * gc4 * gc6 +
        V3_prev * gc5 * gc4 * gc6 - Vc5_prev * gc6 - Vc6_prev * gc5 - gc5 * gc6 * ic5_prev - gc5 * gc6 * ic6_prev) /
        ((R4 + gc5) * (R5 + gc6) * (gc4 + grd));

    // Clamping V3
    V3 = std::clamp(V3, 0.0, 9.0);

    // Memory element update
    ic5_prev = (V2 - Vc5_prev - gc5 * ic5_prev) / (R4 + gc5);
    Vc5_prev = V2 - ic5_prev * R4;
    ic6_prev = (V2 - Vc6_prev - gc6 * ic6_prev) / (R5 + gc6);
    Vc6_prev = V2 - ic5_prev * R5;

    // Distortion filter
    double V4 = (R7 * V4_prev * gc8 + V3 + V4_prev * gc7 * gc8 - Vc7_prev - gc7 * ic7_prev) /
        (R7 * gc8 + R7 * gr8 + gc7 * gc8 + gc7 * gr8 + 1.0);

    ic7_prev = gr8 * V4 + gc8 * (V4 - V4_prev);
    Vc7_prev = V3 - V4 - R7 * ic7_prev;
    V4_prev = V4;

    // Waveshaper
    double V5 = V4;
    if (V4 <= -1.7) {
        V5 = -1.0;
    }
    else if (V4 > -1.75 && V4 < -0.3) {
        V4 += 0.3;
        V5 = V4 + (V4 * V4) / (4.0 * (1.0 - 0.3)) - 0.3;
    }
    else if (V4 > 0.9 && V4 < 1.1) {
        V4 -= 0.9;
        V5 = V4 - (V4 * V4) / (4.0 * (1.0 - 0.9)) + 0.9;
    }
    else if (V4 > 1.1) {
        V5 = 1.0;
    }

    // Tone filter stage
    double V6 = (R11 * V5 * gc9 + R11 * V5 * gr13 - R11 * V5_prev * gc9 + R11 * V6_prev * gc9 + V5 * gc10 * gc9 +
        V5 * gc10 * gr13 + V5 - V5_prev * gc10 * gc9 + V6_prev * gc10 * gc9 - Vc10_prev - gc10 * ic10_prev) /
        ((R11 + gc10) * (gc9 + gr13));

    ic10_prev = gr13 * (V6 - V5) + gc9 * (V6 - V5 - V6_prev + V5_prev);
    V6_prev = V6;
    V5_prev = V5;
    Vc10_prev = V5 - ic10_prev * R11;

    // Output calculation
    double Vout = (R15 * Vc11_prev + R15 * gc11 * ic11_prev + Rt * V6 - Rt * Vc12_prev - Rt * gc12 * ic11_prev +
        V6 * gc11 + Vc11_prev * gc12 - Vc12_prev * gc11) /
        (R15 + Rt + gc11 + gc12);

    ic11_prev = (V6 - Vc12_prev - Vout - gc12 * ic11_prev) / (R15 + gc12);
    Vc11_prev += gc11 * (ic11_prev + ic11_prev);
    Vc12_prev += gc12 * (ic11_prev + ic11_prev);
    ic11_prev = ic11_prev;

    return Vout*Volume/2.;
}

void Waveshaper::process(juce::dsp::AudioBlock<float>& block) {

    for (int sample = 0;sample < block.getNumSamples();++sample) {
        float x = (block.getSample(0, sample) + block.getSample(1, sample)) / 2.0;
        float y = solve(x);
        block.setSample(0, sample, y);
        block.setSample(1, sample, y);
    }

}