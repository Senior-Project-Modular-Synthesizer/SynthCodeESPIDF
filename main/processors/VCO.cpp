#include "VCO.hpp"
#include "esp_log.h"

VCO::VCO() {
    // Initialize any necessary state here
}

VCO::~VCO() {
    // Clean up any resources if necessary
}

void VCO::process(QuadInputBuffer& input, QuadOutputBuffer& output) {
    int sample_count = 0;
    float running_average[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    int count = 0;
    while (true) {
        QuadIntSample sample = input.nextIntSample();
        if (count % 10000 == 0) {
            ESP_LOGI("VCO", "Processing sample %d %d %d %d", sample.channels[0], sample.channels[1], sample.channels[2], sample.channels[3]);
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

int VCO::blockSize() const {
    return 64; // Example block size
}  

std::variant<std::map<std::string, std::pair<UIElement, void*>>, CustomUI> VCO::getUIType() const {
    std::map<std::string, std::pair<UIElement, void*>> ui_map;
    ui_map[std::string("Alpha")] = std::make_pair(UIElement::SLIDER, static_cast<void*>(const_cast<float*>(&alpha)));
    return ui_map;
}   

void registerBasicProcessors() {
    ProcessorFactory::instance().registerProcessor("VCO", []() {
        return std::make_unique<VCO>();
    });
    ProcessorFactory::instance().registerProcessor("HighPass", []() {
        return std::make_unique<HighPass>();
    });
}

