#pragma once
#include "Processor.hpp"
#include "processor_factory.hpp"

#include <variant>
#include <map>
#include <string>
#include <memory>

class LFO : public Processor {
    public:
        LFO();
        ~LFO();

        void process(QuadInputBuffer& input, QuadOutputBuffer& output) override;
        int blockSize() const override;
        const UIElement* getUIType() const override;
    private:
        float fm = 50.0f;
        float freq = 10.0f;
    
        const UIElement ui_map[6] = {
            { SLIDER, "Freq * 5", 0, 100, 1, &fm },
            { NUMBER, "Freq", 0, 20, 0, &freq},
            EMPTY_ELEMENT,
            EMPTY_ELEMENT,
            EMPTY_ELEMENT,
            EMPTY_ELEMENT
        };
};

class VCO : public Processor {
    public:
        VCO();
        ~VCO();

        void process(QuadInputBuffer& input, QuadOutputBuffer& output) override;
        int blockSize() const override;
        const UIElement* getUIType() const override;
    private:
        float fm = 10.0f;
        float tune = 10.0f;
    
        const UIElement ui_map[6] = {
            { SLIDER, "FM", 0, 100, 1, &fm },
            { SLIDER, "Tune", 0, 100, 1, &tune },
            EMPTY_ELEMENT,
            EMPTY_ELEMENT,
            EMPTY_ELEMENT,
            EMPTY_ELEMENT
        };
};

void phase_to_sample(QuadSample& sample, float phase);

void register_oscillator();