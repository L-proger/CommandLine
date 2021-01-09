#pragma once

#include <string>

namespace CommandLine {

enum class ArgumentType {
    SingleValue,
    SingleOrNoValue,
    MultipleValues
};

class ArgumentDescription final {
public:
    ArgumentDescription(const std::string& name, const std::string& helpText, ArgumentType type = ArgumentType::SingleValue) : _name(name), _helpText(helpText), _type(type) { }
    const std::string& name() const {
        return _name;
    }
    const std::string& helpText() const {
        return _helpText;
    }
    const ArgumentType& type() const {
        return _type;
    }
    bool canBeEmpty() const {
        return _type == ArgumentType::MultipleValues || _type == ArgumentType::SingleOrNoValue;
    }
private:
    std::string _name;
    std::string _helpText;
    ArgumentType _type;
};

}
