#pragma once

#include <type_traits>
#include <string>
#include "CommandLineException.h"

namespace CommandLine {
    template<typename T>
    class ValueConverter {
    public:
        static T convert(const std::string& value) {
            if constexpr (std::is_integral_v<T> && std::is_signed_v<T>) {
                auto result = std::strtoll(value.c_str(), nullptr, 0);
                if ((result < (std::numeric_limits<T>::min)()) || (result > (std::numeric_limits<T>::max)())) {
                    throw CommandLine::Exception("Value is out of range");
                }
                return static_cast<T>(result);
            }
            else  if constexpr (std::is_integral_v<T> && !std::is_signed_v<T>) {
                auto result = std::strtoull(value.c_str(), nullptr, 0);
                if ((result < (std::numeric_limits<T>::min)()) || (result > (std::numeric_limits<T>::max)())) {
                    throw CommandLine::Exception("Value is out of range");
                }
                return static_cast<T>(result);
            }
            else {
                throw CommandLine::Exception("ValueConverter: converter not found for type T");
            }
            
        }
    };

    template<>
    class ValueConverter<bool> {
    public:
        static bool convert(const std::string& value) {
            return (value == "true") || (value == "True") || (value == "TRUE");
        }
    };


}
