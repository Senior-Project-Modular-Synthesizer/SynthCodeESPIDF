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
        float fm = 0.01f;
        float tune = 0.01f;
    
        const UIElement ui_map[6] = {
            EMPTY_ELEMENT,
            EMPTY_ELEMENT,
            EMPTY_ELEMENT,
            EMPTY_ELEMENT,
            EMPTY_ELEMENT,
            EMPTY_ELEMENT
        };
};

void registerBasicProcessors();