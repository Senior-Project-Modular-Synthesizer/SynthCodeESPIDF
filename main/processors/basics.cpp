#include "basics.hpp"
#include "esp_log.h"

LowPass::LowPass() {
    // Initialize any necessary state here
}

LowPass::~LowPass() {
    // Clean up any resources if necessary
}

void LowPass::process(QuadInputBuffer& input, QuadOutputBuffer& output) {
    int sample_count = 0;
    float running_average[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    int count = 0;
    while (true) {
        QuadIntSample sample = input.nextIntSample();
        if (count % 10000 == 0) {
            ESP_LOGI("LowPass", "Processing sample %d %d %d %d", sample.channels[0], sample.channels[1], sample.channels[2], sample.channels[3]);
        }
        count++;
        for (int i = 0; i < 4; i++) {
            float channel_float = (float)(sample.channels[i]) / (float)(0x7FFFFF);
            running_average[i] = alpha * channel_float + (1.0f - alpha) * running_average[i];
            sample.channels[i] = (int32_t)(running_average[i] * 0x7FFFFF);
        }
        output.pushIntSample(sample);
    }
}

int LowPass::blockSize() const {
    return 64; // Example block size
}

HighPass::HighPass() {
    // Initialize any necessary state here
}

HighPass::~HighPass() {
    // Clean up any resources if necessary
}

void HighPass::process(QuadInputBuffer& input, QuadOutputBuffer& output) {
    int sample_count = 0;
    float running_average[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    while (true) {
        QuadIntSample sample = input.nextIntSample();
        for (int i = 0; i < 4; i++) {
            float channel_float = (float)(sample.channels[i]) / (float)(0x7FFFFF);
            running_average[i] = alpha * channel_float + (1.0f - alpha) * running_average[i];
            sample.channels[i] = (int32_t)((channel_float - running_average[i]) * 0x7FFFFF);
        }
        output.pushIntSample(sample);
    }
}

int HighPass::blockSize() const {
    return 64; // Example block size
}

const UIElement* HighPass::getUIType() const {
    return ui_map;
}   

const UIElement* LowPass::getUIType() const {
    return ui_map;
}   

void registerBasicProcessors() {
    ProcessorFactory::instance().registerProcessor("LowPass", []() {
        return std::make_unique<LowPass>();
    });
    ProcessorFactory::instance().registerProcessor("HighPass", []() {
        return std::make_unique<HighPass>();
    });
}

