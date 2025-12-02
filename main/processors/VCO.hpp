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
        float alpha = 0.01f;
};

// class VCA : public Processor {
//     public:
//         VCA();
//         ~VCA();

//         void process(QuadInputBuffer& input, QuadOutputBuffer& output) override;
//         int blockSize() const override;
//         std::variant<std::map<std::string, std::pair<UIElement, void*>>, CustomUI> getUIType() const override;
//     private:
//         float alpha = 0.01f;
// };

void registerBasicProcessors();