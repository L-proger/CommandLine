#pragma once

#include "CommandLineException.h"
#include <algorithm>
#include <string>
#include <vector>

namespace CommandLine {

enum class OptionType {
    NoValue,
    SingleValue,
    SingleOrNoValue,
    MultipleValues
};

class OptionDescription{
public:
    OptionDescription(const std::string& name, const std::string& helpText, OptionType type = OptionType::NoValue) :  _helpText(helpText), _type(type) {
        if(!isLongOption(name)){
            throw CommandLine::Exception("Long option name expected");
        }
        _names.push_back(name);
    }

    OptionDescription& alias(const std::string& name) {
        if(!isShortOption(name)){
            throw CommandLine::Exception("Short option name expected");
        }
        _names.push_back(name);
        return *this;
    }

    const OptionType& type() const {
        return _type;
    }

    const std::vector<std::string>& names() const {
        return _names;
    }

    const std::string& helpText() const {
        return _helpText;
    }

    bool match(const std::string& key) const {
        return std::find(_names.begin(), _names.end(), key) != _names.end();
    }

    bool match(const OptionDescription& rhs) const {
        return std::any_of(rhs.names().begin(), rhs.names().end(), [this](auto& key){ return match(key); });
    }

    static bool isShortOption(const std::string& name) {
        if(name.size() < 2){
            return false;
        }
        if(hasForbiddenChars(name)){
            return false;
        }
        return name[0] == '-' && name [1] != '-';
    }

    static bool isLongOption(const std::string& name) {
        if(name.size() < 3){
            return false;
        }
        if(hasForbiddenChars(name)){
            return false;
        }
        return name[0] == '-' && name [1] == '-' && name [2] != '-';
    }

    static bool isValidOption(const std::string& name) {
        return isShortOption(name) || isLongOption(name);
    }
private:
    static bool hasForbiddenChars(const std::string& name) {
        const char forbiddenCharacter[] = { '<', '>', '(', ')', ' ', ':', '=', '!'};
        for(auto c : forbiddenCharacter){
            if(name.find(c) != std::string::npos){
                return true;
            }
        }
        return false;
    }
    std::vector<std::string> _names;
    std::string _helpText;
    OptionType _type;
};

}
