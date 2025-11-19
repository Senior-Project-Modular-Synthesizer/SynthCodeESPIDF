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

std::unique_ptr<Processor> ProcessorFactory::createProcessor(const std::string& name) {
    auto it = registry.find(name);
    if (it != registry.end()) {
        return it->second();
    }
    return nullptr;
}

void ProcessorFactory::registerProcessor(const std::string& name, std::function<std::unique_ptr<Processor>()> creator) {
    registry[name] = creator;
}