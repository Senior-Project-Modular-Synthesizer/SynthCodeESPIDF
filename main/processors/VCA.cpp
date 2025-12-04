
#include "VCA.hpp"
#include "../approx.h"
#include "esp_log.h"


VCA::VCA() {
    // Initialize any necessary state here
}

VCA::~VCA() {
    // Clean up any resources if necessary
}

/*
 * Linear uses range from 0 to 2
 */
void VCA::process(QuadInputBuffer& input, QuadOutputBuffer& output) {
    QuadIntSample sample = input.nextSample();
    while (true) {
        // Calculate gain every 64 samples
        float gain;
        if (sample.channels[3] / 10.0f > max_v)
            gain = 2;
        else if (sample.channels[3] / 10.0f < min_v)
            gain = 0;
        else
            gain = 2.0f / ((max_v - min_v) / 10.0f) * (sample.channels[3] - min_v)

        for (int i = 0; i < 64; i++){
            sample.channels[0] = sample.channels[0] * gain;
            sample.channels[1] = sample.channels[1] * gain;
            sample.channels[2] = sample.channels[2] * gain;
            
            output.pushSample(sample);
            sample = input.nextSample();
        }
    }
}

int VCA::blockSize() const {
    return 64; // Example block size
}  

const UIElement* VCA::getUIType() const {
    return ui_map;
}


bipolar_VCA::bipolar_VCA() {
    // Initialize any necessary state here
}

bipolar_VCA::~bipolar_VCA() {
    // Clean up any resources if necessary
}

/*
 * Bipolar uses range from -2 to 2
 */
void bipolar_VCA::process(QuadInputBuffer& input, QuadOutputBuffer& output) {
    QuadIntSample sample = input.nextSample();
    while (true) {
        // Calculate gain every 64 samples
        float gain = 2.0f * sample.channels[3];

        for (int i = 0; i < 64; i++){
            sample.channels[0] = sample.channels[0] * gain;
            sample.channels[1] = sample.channels[1] * gain;
            sample.channels[2] = sample.channels[2] * gain;
            
            output.pushSample(sample);
            sample = input.nextSample();
        }
    }
}

int bipolar_VCA::blockSize() const {
    return 64; // Example block size
}  

const UIElement* bipolar_VCA::getUIType() const {
    return ui_map;
}

void register_VCA() {
    ProcessorFactory::instance().registerProcessor("VCA", []() {
        return std::make_unique<VCA>();
    });

    ProcessorFactory::instance().registerProcessor("Bipo VCA", []() {
        return std::make_unique<VCA>();
    });
}

