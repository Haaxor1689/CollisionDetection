#pragma once

#include <array>

namespace MyEngine {

class Matrix {
    // Helper row class
    class Row {
        float* data;
        size_t x;

        friend class Matrix;
        Row(float* data, size_t x) : data(data), x(x) {}

    public:
        float& operator[](size_t y) {
            return data[3 * y + x];
        }
    };
    
    std::array<float, 9> data;

public:
    // Access operators
    Row operator[](size_t x) {
        return Row(data.data(), x);
    }
};

}