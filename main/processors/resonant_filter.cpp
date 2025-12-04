
#include "resonanant_filter.hpp"
#include "../approx.h"
#include "esp_log.h"


resonanant_filter::resonanant_filter() {
    // Initialize any necessary state here
}

resonanant_filter::~resonanant_filter() {
    // Clean up any resources if necessary
}

/*
 *  Delta = 2^(Tune + V/oct + FM * FM_in))
 *  
 *  input.channels: 
 *          0 - V/oct - Voltage per octave
 *          1 - FM_in - Input frequency that we modulate by
 */
void resonanant_filter::process(QuadInputBuffer& input, QuadOutputBuffer& output) {
    float phase = 0.0f;

    while (true) {
        
    }
}

int resonanant_filter::blockSize() const {
    return 64; // Example block size
}  

const UIElement* resonanant_filter::getUIType() const {
    return ui_map;
}   

void register_resonanant_filter() {
    ProcessorFactory::instance().registerProcessor("Resonant", []() {
        return std::make_unique<resonanant_filter>();
    });
}

