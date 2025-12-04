#pragma once
#include "Processor.hpp"
#include "processor_factory.hpp"

#include <variant>
#include <map>
#include <string>
#include <memory>

class VCA : public Processor {
    public:
        VCA();
        ~VCA();

        void process(QuadInputBuffer& input, QuadOutputBuffer& output) override;
        int blockSize() const override;
        const UIElement* getUIType() const override;
    private:
        float min_v = 0.0f;
        float max_v = 10.0f;
        
        const UIElement ui_map[6] = {
            { SLIDER, "Min Voltage", -10, 10, 1, &min_v },
            { SLIDER, "Max Voltage", -10, 10, 1, &max_v },
            EMPTY_ELEMENT,
            EMPTY_ELEMENT,
            EMPTY_ELEMENT,
            EMPTY_ELEMENT
        };
};

class bipolar_VCA : public Processor {
    public:
        bipolar_VCA();
        ~bipolar_VCA();

        void process(QuadInputBuffer& input, QuadOutputBuffer& output) override;
        int blockSize() const override;
        const UIElement* getUIType() const override;
    private:
        float min_v = 0.0f;
        float max_v = 10.0f;
        
        const UIElement ui_map[6] = {
            { NUMBER, "Min Voltage", 0, 0, 0, &min_v },
            { NUMBER, "Max Voltage", 0, 0, 0, &max_v },
            EMPTY_ELEMENT,
            EMPTY_ELEMENT,
            EMPTY_ELEMENT,
            EMPTY_ELEMENT
        };
};

void register_VCA();