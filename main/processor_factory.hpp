#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

#include "Processor.hpp"

class ProcessorFactory {
public:
    static ProcessorFactory& instance() {
        static ProcessorFactory factory;
        return factory;
    }
    std::unique_ptr<Processor> createProcessor(const std::string& name);
private:
    ProcessorFactory() = default;
    ~ProcessorFactory() = default;

    ProcessorFactory(const ProcessorFactory&) = delete;
    ProcessorFactory& operator=(const ProcessorFactory&) = delete;

    std::unordered_map<std::string, std::function<std::unique_ptr<Processor>()>> registry;
};