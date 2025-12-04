
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
 *  Delta = 2^(Tune + V/oct + FM * FM_in))
 *  
 *  input.channels: 
 *          0 - V/oct - Voltage per octave
 *          1 - FM_in - Input frequency that we modulate by
 */
void VCA::process(QuadInputBuffer& input, QuadOutputBuffer& output) {
    float phase = 0.0f;

    while (true) {
        
    }
}

int VCA::blockSize() const {
    return 64; // Example block size
}  

const UIElement* VCA::getUIType() const {
    return ui_map;
}   

void register_VCA() {
    ProcessorFactory::instance().registerProcessor("VCA", []() {
        return std::make_unique<VCA>();
    });
}

