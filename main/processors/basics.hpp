#pragma once
#include "Processor.hpp"
#include "processor_factory.hpp"

#include <variant>
#include <map>
#include <string>
#include <memory>

class Filter : public Processor {
    public:
        Filter();
        ~Filter();

        void process(QuadInputBuffer& input, QuadOutputBuffer& output) override;
        int blockSize() const override;
        std::variant<std::map<std::string, std::pair<UIElement, void*>>, CustomUI> getUIType() const override;
    private:
        float alpha = 1.0f;
};

void registerBasicProcessors();