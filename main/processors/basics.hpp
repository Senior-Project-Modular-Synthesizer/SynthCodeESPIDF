#pragma once
#include "Processor.hpp"
#include "processor_factory.hpp"

#include <variant>
#include <map>
#include <string>
#include <memory>

class LowPass : public Processor {
    public:
        LowPass();
        ~LowPass();

        void process(QuadInputBuffer& input, QuadOutputBuffer& output) override;
        int blockSize() const override;
        const UIElement* getUIType() const override;
    private:
        float alpha = 0.01f;

        const UIElement ui_map[6] = {
            { SLIDER, "Alpha", 0, 100, 50, &alpha },
            EMPTY_ELEMENT,
            EMPTY_ELEMENT,
            EMPTY_ELEMENT,
            EMPTY_ELEMENT,
            EMPTY_ELEMENT
        };
};

class HighPass : public Processor {
    public:
        HighPass();
        ~HighPass();

        void process(QuadInputBuffer& input, QuadOutputBuffer& output) override;
        int blockSize() const override;
        const UIElement* getUIType() const override;
    private:
        float alpha = 0.01f;

        const UIElement ui_map[6] = {
            { SLIDER, "Alpha", 0, 100, 50, &alpha },
            EMPTY_ELEMENT,
            EMPTY_ELEMENT,
            EMPTY_ELEMENT,
            EMPTY_ELEMENT,
            EMPTY_ELEMENT
        };
};

void registerBasicProcessors();