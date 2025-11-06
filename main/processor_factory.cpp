#include "processor_factory.hpp"

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

#include "Processor.hpp"

ProcessorFactory::ProcessorFactory() {
    this->registry = {};
}

ProcessorFactory::~ProcessorFactory() {
    this->registry.clear();
}

ProcessorFactory& ProcessorFactory::instance() {
    static ProcessorFactory factory;
    return factory;
}

std::unique_ptr<Processor> ProcessorFactory::createProcessor(const std::string& name) {
    auto it = registry.find(name);
    if (it != registry.end()) {
        return it->second();
    }
    return nullptr;
}