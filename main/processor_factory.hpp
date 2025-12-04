#pragma once

#include <memory>
#include <string>
#include <map>
#include <functional>

#include "Processor.hpp"

class ProcessorFactory {
public:
    static ProcessorFactory& instance() {
        static ProcessorFactory factory;
        return factory;
    }
    std::unique_ptr<Processor> createProcessor(const std::string& name);

    void registerProcessor(const std::string& name, std::function<std::unique_ptr<Processor>()> creator);

private:
    ProcessorFactory();
    ~ProcessorFactory();

    ProcessorFactory(const ProcessorFactory&);
    ProcessorFactory& operator=(const ProcessorFactory&);

    std::map<std::string, std::function<std::unique_ptr<Processor>()>> registry;
};