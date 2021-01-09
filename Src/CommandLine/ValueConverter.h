#pragma once

#include <type_traits>
#include <string>

namespace CommandLine {
    template<typename T, bool isIntegral = std::is_integral_v<T>>
    class ValueConverter {
    public:
        static T convert(const std::string& /*value*/) {
            throw ParserException("ValueConverter: converter not found for type T");
        }
    };

    template<typename T>
    class ValueConverter<T, true> {
    public:
        static T convert(const std::string& value) {
            if constexpr(std::is_signed_v<T>){
                auto result = std::strtoll(value.c_str(), nullptr, 0);
                if((result < (std::numeric_limits<T>::min)()) || (result > (std::numeric_limits<T>::max)())){
                    throw ParserException("Value is out of range");
                }
                return static_cast<T>(result);
            }else{
                auto result = std::strtoull(value.c_str(), nullptr, 0);
                if((result < (std::numeric_limits<T>::min)()) || (result > (std::numeric_limits<T>::max)())){
                    throw ParserException("Value is out of range");
                }
                return static_cast<T>(result);
            }
        }
    };
}
