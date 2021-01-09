#pragma once

#include "ArgumentDescription.h"
#include <optional>
#include <vector>
#include <string>

namespace CommandLine {

class Argument final {
public:
    friend class Parser;

    Argument(const ArgumentDescription& description) : _description(description){}

    const ArgumentDescription& description() const {
        return _description;
    }
    const std::string& value() const {
        return *_value->begin();
    }
    const std::vector<std::string>& values() const {
        return *_value;
    }
    bool isSet() const {
        return _value.has_value();
    }
    operator std::string() {
        return value();
    }
    operator std::optional<std::string>() {
        if(_value.has_value() && !_value->empty()){
            return *_value->begin();
        }
        return std::nullopt;
    }
private:
    std::optional<std::vector<std::string>> _value;
    ArgumentDescription _description;
};

}
