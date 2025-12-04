
#include "LFO.hpp"
#include "../approx.h"
#include "esp_log.h"


LFO::LFO() {
    // Initialize any necessary state here
}

LFO::~LFO() {
    // Clean up any resources if necessary
}

/*
 *  Delta = 2^(Tune + V/oct + FM * FM_in))
 *  
 *  input.channels: 
 *          0 - V/oct - Voltage per octave
 *          1 - FM_in - Input frequency that we modulate by
 */
void LFO::process(QuadInputBuffer& input, QuadOutputBuffer& output) {
    float phase = 0.0f;

    while (true) {
        
    }
}

int LFO::blockSize() const {
    return 64; // Example block size
}  

const UIElement* LFO::getUIType() const {
    return ui_map;
}   

void register_LFO() {
    ProcessorFactory::instance().registerProcessor("LFO", []() {
        return std::make_unique<LFO>();
    });
}

