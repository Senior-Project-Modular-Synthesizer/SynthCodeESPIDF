#include "oscillator.hpp"
#include "../approx.h"
#include "esp_log.h"
#include "peripheral_cfg.h"

/**
 * Fills in sample using the phase
 */
void phase_to_sample(QuadIntSample sample, float phase) {
    // Above each effect is a formula to paste into Desmos to visualize it

    // \left\{0<x<1:\sin\left(2\pi x\right)\right\}
    sample.channels[0] = lookup_sin(phase); // sine

    // \left\{0\le x<0.5\ :\ 1,\ 0.5\le x<1:\ -1\right\}
    sample.channels[1] = phase > 0.5 ? 1.0 : -1.0; // square

    // \left\{0\le x<0.5\ :\ 2x,\ 0.5\le x<1:2\left(x-1\right)\right\}
    sample.channels[2] = phase < .5 ? (phase * 2) : ((phase - 1) * 2); // sawtooth  

    // triangle requires more logic to put it in phase with sine
    // \left\{0\le x<0.25\ :\ 4x,\ 0.25\le x<0.75\ :\ -4x+2,\ 0.75\le x<1:4\left(x-1\right)\right\}
    if (phase < 0.25) 
        sample.channels[3] =  (4 * phase);
    else if (0.25 <= phase && phase < 0.75) 
        sample.channel[3]s = -(4 * phase) + 2;
    else 
        sample.channels[3] =   4 * (phase - 1);
    // (This is my attempt to make this more readable,
    //  essentially this just makes the triangle wave follow sine(2pix))
}

LFO::LFO() {
    // Initialize any necessary state here
}

LFO::~LFO() {
    // Clean up any resources if necessary
}

/*
 * Delta = sample rate (48000) / freq 
 */
void LFO::process(QuadInputBuffer& input, QuadOutputBuffer& output) {
    float phase = 0.0f;

    while (true) {
        QuadIntSample sample = input.nextSample();
        freq = fm / 5.0f;
        
        float delta = I2S_SAMPLE_RATE / freq;

        // Phase = delta, get only phase's fractional part
        phase += delta;
        phase %= 1;
    
        phase_to_sample(sample, phase);

        output.pushSample(sample);
    }
}

int LFO::blockSize() const {
    return 64; // Example block size
}  

const UIElement* LFO::getUIType() const {
    return ui_map;
}


VCO::VCO() {
    // Initialize any necessary state here
}

VCO::~VCO() {
    // Clean up any resources if necessary
}

/*
 *  Delta = 2^(Tune + V/oct + FM * FM_in))
 *  
 *  input.channels: 
 *          0 - V/oct - Voltage per octave
 *          1 - FM_in - Input frequency that we modulate by
 */
void VCO::process(QuadInputBuffer& input, QuadOutputBuffer& output) {
    float phase = 0.0f;

    while (true) {
        QuadIntSample sample = input.nextSample();

        // Pass inputs to sample
        float voct = sample.channels[3];
        float fm_in = sample.channels[4];
        
        // Delta = 2 ^ ((fm * fm_in) + voct + tune);
        float exp = ((fm / 1000.0f) * fm_in) + voct + (tune / 1000.0f);
        float delta = lookup_two_pow(exp);

        // Phase = delta, get only phase's fractional part
        phase += delta;
        phase %= 1;

        phase_to_sample(sample, phase);
    
        output.pushSample(sample);
    }
}

int VCO::blockSize() const {
    return 64; // Example block size
}  

const UIElement* VCO::getUIType() const {
    return ui_map;
}   


void register_oscillator() {
    ProcessorFactory::instance().registerProcessor("VCO", []() {
        return std::make_unique<VCO>();
    });
    ProcessorFactory::instance().registerProcessor("LFO", []() {
        return std::make_unique<LFO>();
    });
}

