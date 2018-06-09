#pragma once

#include <fstream>
#include <string>

namespace Utility {

inline std::string LoadFileToString(const std::string& fileName) {
    std::ifstream infile{ fileName };

    if (!infile) {
        throw std::runtime_error("File " + fileName + " does not exists");
    }

    return { std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>() };
}

} // namespace Utility
