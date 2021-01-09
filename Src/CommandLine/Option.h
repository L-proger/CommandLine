#pragma once

#include "CommandLineException.h"
#include "OptionDescription.h"
#include "ValueConverter.h"

#include <string>
#include <vector>
#include <optional>

namespace CommandLine {

class Option {
public:
    friend class Parser;

    Option(OptionDescription description) : _description(description) {}

    bool isSet() const {
        return _values.has_value();
    }

    const std::string& value() const {
        if (!isSet() || _values->empty()) {
            throw CommandLine::Exception("No value for option: " + _description.names()[0]);
        }
        return *_values->begin();
    }

    template<typename T>
    T value() const {
        return ValueConverter<T>::convert(value());
    }

    template<typename T>
    std::optional<T> valueOptional() const {
        if(isSet()){
            return ValueConverter<T>::convert(value());
        }
        return std::nullopt;
    }

    std::string valueOrEmpty() const {
        if (!isSet() || _values->empty()) {
            return "";
        }
        return *_values->begin();
    }

    std::vector<std::string> values() const {
        if (!isSet()) {
            if(_description.type() == OptionType::SingleOrNoValue || _description.type() == OptionType::NoValue){
                return {};
            }
            throw CommandLine::Exception("No values for option: " + _description.names()[0] + " [" + _description.helpText() + "]");
        }
        return *_values;
    }

    template<typename T>
    std::vector<T> values() const {
        auto strValues = values();
        std::vector<T> result;
        for(const auto& val : strValues){
            result.push_back(ValueConverter<T>::convert(val));
        }
        return result;
    }

    const OptionDescription& description() const {
        return _description;
    }
private:
    std::optional<std::vector<std::string>> _values;
    OptionDescription _description;
};

}
