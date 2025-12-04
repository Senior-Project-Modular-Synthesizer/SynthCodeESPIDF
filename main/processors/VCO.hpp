#pragma once
#include "Processor.hpp"
#include "processor_factory.hpp"

#include <variant>
#include <map>
#include <string>
#include <memory>

class VCO : public Processor {
    public:
        VCO();
        ~VCO();

        void process(QuadInputBuffer& input, QuadOutputBuffer& output) override;
        int blockSize() const override;
        std::variant<std::map<std::string, std::pair<UIElement, void*>>, CustomUI> getUIType() const override;
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

void registerBasicProcessors();