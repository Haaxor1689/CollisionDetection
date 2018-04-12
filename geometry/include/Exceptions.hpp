#pragma once

#include <string>
#include <exception>

namespace Geometry {
template <size_t Dimension>
struct DimensionsException : public std::exception {
    DimensionsException(std::string className, std::string methodName)
        : message("Can't call " + methodName + " on " + className + " with dimension " + std::to_string(Dimension)) {}

    char const* what() const noexcept override {
        return message.c_str();
    }

private:
    std::string message;
};
} // namespace Geometry