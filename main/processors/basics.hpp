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
        UIElement* getUIType() const override;
    private:
        int alpha = 0.01f;
};

class HighPass : public Processor {
    public:
        HighPass();
        ~HighPass();

        void process(QuadInputBuffer& input, QuadOutputBuffer& output) override;
        int blockSize() const override;
        UIElement* getUIType() const override;
    private:
        int alpha = 0.01f;
};

void registerBasicProcessors();