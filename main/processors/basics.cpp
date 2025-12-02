#include "basics.hpp"

LowPass::LowPass() {
    // Initialize any necessary state here
}

LowPass::~LowPass() {
    // Clean up any resources if necessary
}

void LowPass::process(QuadInputBuffer& input, QuadOutputBuffer& output) {
    int sample_count = 0;
    float running_average[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    while (true) {
        QuadIntSample sample = input.nextIntSample();
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

UIElement* HighPass::getUIType() const {
    UIElement ui_map[6] = {
        { UITypes.SLIDER, "Alpha", 0, 100, 50, &alpha },
        EMPTY_ELEMENT,
        EMPTY_ELEMENT,
        EMPTY_ELEMENT,
        EMPTY_ELEMENT,
        EMPTY_ELEMENT
    };
    return ui_map;
}   

UIElement* LowPass::getUIType() const {
    UIElement ui_map[6] = {
        { UITypes.SLIDER, "Alpha", 0, 100, 50, &alpha },
        EMPTY_ELEMENT,
        EMPTY_ELEMENT,
        EMPTY_ELEMENT,
        EMPTY_ELEMENT,
        EMPTY_ELEMENT
    };
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

