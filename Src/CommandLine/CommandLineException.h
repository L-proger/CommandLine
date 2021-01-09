#pragma once

#include <exception>
#include <string>

namespace CommandLine {

class Exception : std::exception {
public:
    Exception(const std::string& message) : _message(message){}
    char const* what() const noexcept override{
        return _message.empty() ? nullptr : _message.c_str();
    }
private:
    std::string _message;
};

}
