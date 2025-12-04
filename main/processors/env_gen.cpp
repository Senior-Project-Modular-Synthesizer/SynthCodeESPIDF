
#include "env_gen.hpp"
#include "../approx.h"
#include "esp_log.h"


env_gen::env_gen() {
    // Initialize any necessary state here
}

env_gen::~env_gen() {
    // Clean up any resources if necessary
}

/*
 *  Delta = 2^(Tune + V/oct + FM * FM_in))
 *  
 *  input.channels: 
 *          0 - V/oct - Voltage per octave
 *          1 - FM_in - Input frequency that we modulate by
 */
void env_gen::process(QuadInputBuffer& input, QuadOutputBuffer& output) {
    float phase = 0.0f;

    while (true) {
        
    }
}

int env_gen::blockSize() const {
    return 64; // Example block size
}  

const UIElement* env_gen::getUIType() const {
    return ui_map;
}   

void register_env_gen() {
    ProcessorFactory::instance().registerProcessor("Env Gen", []() {
        return std::make_unique<env_gen>();
    });
}

