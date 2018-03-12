#pragma once

#include <array>
#include <ostream>

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
    static_assert(width >= 2, "Matrix of size lesser than 2 is not supported.");
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

    int Determinant() const {
        float det = 0;
        for (size_t i = 0; i < width; ++i)
            det += (i % 2 == 0 ? 1 : -1) * data[i] * Minor(0, i).Determinant();
        return det;
    }

    Matrix<width - 1> Minor(size_t row, size_t column) const {
        Matrix<width - 1> minor;
        for (size_t j = 0, l = 0; j < width; ++j) {
            if (j == row)
                continue;
            for (size_t i = 0, k = 0; i < width; ++i) {
                if (i == column)
                    continue;
                minor[k][l] = (*this)[i][j];
                ++k;
            }
            ++l;
        }
        return minor;
    }

    Matrix& Transpose() {
        for (size_t j = 1; j < width; ++j)
            for (size_t i = 0; i < j; ++i)
                std::swap((*this)[i][j], (*this)[j][i]);
        return *this;
    }

    Matrix& Cofactor() {
        for (size_t j = 0; j < width; ++j)
            for (size_t i = 0; i < width; ++i)
                if (i % 2 != j % 2)
                    (*this)[i][j] *= -1; 
        return *this;
    }

    Matrix& Invert() {
        return *this = Matrix::Minors(*this).Cofactor().Transpose() / Determinant();
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

    friend Matrix operator*(Matrix matrix, float value) {
        for (size_t j = 0; j < width; ++j)
            for (size_t i = 0; i < width; ++i)
                matrix[i][j] *= value;
        return matrix;
    }

    friend Matrix operator/(Matrix matrix, float value) {
        for (size_t j = 0; j < width; ++j)
            for (size_t i = 0; i < width; ++i)
                matrix[i][j] /= value;
        return matrix;
    }

    // Output stream operator
    friend std::ostream& operator<<(std::ostream& os, const Matrix& mat) {
        os << "{ ";
        for (size_t i = 0; i < size; ++i)
            os << (i % width == 0 ? "{ " : "") << mat.data[i] << (i % width == width - 1 ? i == size - 1 ? " } " : " }, "  : ", ");
        return os << "}";
    }

    // Static methods
    static Matrix Identity() {
        auto ret = Matrix();
        for (size_t i = 0; i < width; ++i) {
            ret[i][i] = 1.f;
        }
        return ret;
    }

    static Matrix Transposed(Matrix mat) {
        return mat.Transpose();
    }

    static Matrix Minors(const Matrix& matrix) {
        Matrix minors;
        for (size_t j = 0; j < width; ++j)
            for (size_t i = 0; i < width; ++i)
                minors[i][j] = matrix.Minor(j, i).Determinant();
        return minors;
    }

    static Matrix Cofactors(Matrix matrix) {
        return matrix.Cofactor();
    }

    static Matrix Inverted(Matrix matrix) {
        return matrix.Invert();
    }
};

template <>
inline int Matrix<2>::Determinant() const {
    return data[0] * data[3] - data[1] * data [2];
}

}