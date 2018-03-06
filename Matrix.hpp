#pragma once

#include <array>

namespace MyEngine {

// Helper row class
template <typename T, size_t Width>
class Row {
    T* data;
    size_t x;

    template <size_t w>
    friend class Matrix;
    Row(T* data, size_t x) : data(data), x(x) {}
public:
    // Index operators
    T& operator[](size_t y) { return data[Width * y + x]; }
    T operator[](size_t y) const { return data[Width * y + x]; }
};

template <size_t width>
class Matrix {
    // constexpr static size_t width = 3;
    constexpr static size_t size = width * width;

    using ConstRow = const Row<const float, width>;
    using NormalRow = Row<float, width>;

    std::array<float, size> data;
public:
    Matrix() = default;
    Matrix(float value) : data() {
        for (size_t i = 0; i < size; ++i) {
            data[i] = value;
        }
    }

    Matrix(const std::initializer_list<float>& list) : data() {
        if (list.size() != size)
            throw std::invalid_argument("Can't initialize with list of size " + std::to_string(list.size()) + ". Size must be " + std::to_string(size) + ".");

        auto j = data.begin();
        for (auto i : list) {
            *j = i;
            ++j;
        }
    }

    // Index operators
    NormalRow operator[](size_t x) { return { data.data(), x }; }
    ConstRow operator[](size_t x) const { return { data.data(), x }; }
    // Bool operators
    friend bool operator==(const Matrix& lhs, const Matrix& rhs) { return lhs.data == rhs.data; }
    friend bool operator!=(const Matrix& lhs, const Matrix& rhs) { return lhs.data != rhs.data; }
    // Matrix arithmetic operators
    friend Matrix operator+(Matrix lhs, const Matrix& rhs) {
        auto lhsIt = lhs.data.begin();
        auto rhsIt = rhs.data.begin();
        for (; lhsIt != lhs.data.end(); ++lhsIt, ++rhsIt)
            *lhsIt += *rhsIt;
        return lhs;
    }

    friend Matrix operator-(Matrix lhs, const Matrix& rhs) {
        auto lhsIt = lhs.data.begin();
        auto rhsIt = rhs.data.begin();
        for (; lhsIt != lhs.data.end(); ++lhsIt, ++rhsIt)
            *lhsIt -= *rhsIt;
        return lhs;
    }

    friend Matrix operator*(const Matrix& lhs, const Matrix& rhs) {
        Matrix out;
        for (size_t j = 0; j < width; ++j)
            for (size_t i = 0; i < width; ++i) {
                float sum = 0.f;
                for (size_t k = 0; k < width; ++k)
                    sum += lhs[k][j] * rhs[i][k];
                out[i][j] = sum;
            }
        return out;
    }

    // Output stream operator
    friend std::ostream& operator<<(std::ostream& os, const Matrix& mat) {
        os << "{ ";
        for (size_t i = 0; i < size; ++i)
            os << (i % 3 == 0 ? "{ " : "") << mat.data[i] << (i % 3 == 2 ? i == size - 1 ? " } " : " }, "  : ", ");
        return os << "}";
    }

    static Matrix Identity() {
        auto ret = Matrix();
        for (size_t i = 0; i < width; ++i) {
            ret[i][i] = 1.f;
        }
        return ret;
    }
};

}