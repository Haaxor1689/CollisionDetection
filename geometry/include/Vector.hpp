#pragma once

#include <array>
#include <numeric>
#include <cmath>
#include <ostream>
#include <stdexcept>

namespace Geometry {

template <size_t Size>
class Vector {
    static_assert(Size >= 2 && Size <= 4, "Vector of size lesser than 2 or bigger than 4 is not supported.");
    std::array<float, Size> data;

public:
    Vector()
        : Vector(0.f) {}

    Vector(float x)
        : data() {
        for (auto& i : data)
            i = x;
    }

    Vector(std::initializer_list<float>&& list) {
        if (list.size() != Size)
            throw std::invalid_argument("Can't initialize with list of size " + std::to_string(list.size()) + ". Size must be " + std::to_string(Size) + ".");

        auto otherIt = list.begin();
        for (auto thisIt = data.begin(); thisIt != data.end(); ++thisIt, ++otherIt)
            *thisIt = *otherIt;
    }

    float Magnitude() const {
        auto ret = 0.f;
        for (auto& i : data)
            ret += i * i;
        return std::sqrt(ret);
    }

    float& x() { return data[0]; }
    float& y() { return data[1]; }
    float& z() { return data[2]; }

    const float& x() const { return data[0]; }
    const float& y() const { return data[1]; }
    const float& z() const { return data[2]; }

    Vector& Translate(const Vector& other) {
        auto otherIt = other.data.begin();
        for (auto thisIt = data.begin(); thisIt != data.end(); ++thisIt, ++otherIt)
            *thisIt += *otherIt;
        return *this;
    }

    Vector& Rotate(float angle, Vector axis) {
        float const c = cos(angle);
        float const s = sin(angle);

        axis.Normalize();
        Vector temp = axis * (1.f - c);

        Vector result;
        result[0] = (*this)[0] * (c + temp[0] * axis[0]) + (*this)[1] * (temp[1] * axis[0] - s * axis[2]) + (*this)[2] * (temp[2] * axis[0] + s * axis[1]);
        result[1] = (*this)[0] * (temp[0] * axis[1] + s * axis[2]) + (*this)[1] * (c + temp[1] * axis[1]) + (*this)[2] * (temp[2] * axis[1] - s * axis[0]);
        result[2] = (*this)[0] * (temp[0] * axis[2] - s * axis[1]) + (*this)[1] * (temp[1] * axis[2] + s * axis[0]) + (*this)[2] * (c + temp[2] * axis[2]);
        return *this = result;
    }

    Vector& Scale(Vector other) {
        auto otherIt = other.data.begin();
        for (auto thisIt = data.begin(); thisIt != data.end(); ++thisIt, ++otherIt)
            *thisIt *= *otherIt;
        return *this;
    }

    Vector& Normalize() {
        float mag = Magnitude();
        if (mag == 0)
            return *this;
        for (auto& i : data)
            i /= mag;
        return *this;
    }

    Vector& Invert() {
        for (auto& i : data)
            i *= -1.f;
        return *this;
    }

    // Index operators
    float& operator[](size_t index) { return data[index]; }
    const float& operator[](size_t index) const { return data[index]; }
    // Dereference operator
    const float* operator&() const { return &data[0]; }
    // Bool operators
    friend bool operator==(const Vector& lhs, const Vector& rhs) { return lhs.data == rhs.data; }
    friend bool operator!=(const Vector& lhs, const Vector& rhs) { return lhs.data != rhs.data; }
    // Vector arithmetic operators
    friend Vector operator+(Vector lhs, const Vector& rhs) {
        auto rhsIt = rhs.data.begin();
        for (auto lhsIt = lhs.data.begin(); lhsIt != lhs.data.end(); ++lhsIt, ++rhsIt)
            *lhsIt += *rhsIt;
        return lhs;
    }

    friend Vector operator-(Vector lhs, const Vector& rhs) {
        auto rhsIt = rhs.data.begin();
        for (auto lhsIt = lhs.data.begin(); lhsIt != lhs.data.end(); ++lhsIt, ++rhsIt)
            *lhsIt -= *rhsIt;
        return lhs;
    }

    friend Vector operator*(Vector lhs, const Vector& rhs) {
        auto rhsIt = rhs.data.begin();
        for (auto lhsIt = lhs.data.begin(); lhsIt != lhs.data.end(); ++lhsIt, ++rhsIt)
            *lhsIt *= *rhsIt;
        return lhs;
    }

    friend Vector operator/(Vector lhs, const Vector& rhs) {
        auto rhsIt = rhs.data.begin();
        for (auto lhsIt = lhs.data.begin(); lhsIt != lhs.data.end(); ++lhsIt, ++rhsIt)
            *lhsIt /= *rhsIt;
        return lhs;
    }

    friend Vector& operator+=(Vector& lhs, const Vector& rhs) {
        auto rhsIt = rhs.data.begin();
        for (auto lhsIt = lhs.data.begin(); lhsIt != lhs.data.end(); ++lhsIt, ++rhsIt)
            *lhsIt += *rhsIt;
        return lhs;
    }

    friend Vector& operator-=(Vector& lhs, const Vector& rhs) {
        auto rhsIt = rhs.data.begin();
        for (auto lhsIt = lhs.data.begin(); lhsIt != lhs.data.end(); ++lhsIt, ++rhsIt)
            *lhsIt -= *rhsIt;
        return lhs;
    }

    friend Vector& operator*=(Vector& lhs, const Vector& rhs) {
        auto rhsIt = rhs.data.begin();
        for (auto lhsIt = lhs.data.begin(); lhsIt != lhs.data.end(); ++lhsIt, ++rhsIt)
            *lhsIt *= *rhsIt;
        return lhs;
    }

    friend Vector& operator/=(Vector& lhs, const Vector& rhs) {
        auto rhsIt = rhs.data.begin();
        for (auto lhsIt = lhs.data.begin(); lhsIt != lhs.data.end(); ++lhsIt, ++rhsIt)
            *lhsIt /= *rhsIt;
        return lhs;
    }

    // Scalar arithmetic operators
    friend Vector operator+(Vector vec, float scalar) {
        for (auto lhsIt = vec.data.begin(); lhsIt != vec.data.end(); ++lhsIt)
            *lhsIt += scalar;
        return vec;
    }

    friend Vector operator-(Vector vec, float scalar) {
        for (auto lhsIt = vec.data.begin(); lhsIt != vec.data.end(); ++lhsIt)
            *lhsIt -= scalar;
        return vec;
    }

    friend Vector operator*(Vector vec, float scalar) {
        for (auto lhsIt = vec.data.begin(); lhsIt != vec.data.end(); ++lhsIt)
            *lhsIt *= scalar;
        return vec;
    }

    friend Vector operator/(Vector vec, float scalar) {
        for (auto lhsIt = vec.data.begin(); lhsIt != vec.data.end(); ++lhsIt)
            *lhsIt /= scalar;
        return vec;
    }

    friend Vector operator+(float scalar, Vector vec) {
        for (auto lhsIt = vec.data.begin(); lhsIt != vec.data.end(); ++lhsIt)
            *lhsIt += scalar;
        return vec;
    }

    friend Vector operator-(float scalar, Vector vec) {
        for (auto lhsIt = vec.data.begin(); lhsIt != vec.data.end(); ++lhsIt)
            *lhsIt -= scalar;
        return vec;
    }

    friend Vector operator*(float scalar, Vector vec) {
        for (auto lhsIt = vec.data.begin(); lhsIt != vec.data.end(); ++lhsIt)
            *lhsIt *= scalar;
        return vec;
    }

    friend Vector operator/(float scalar, Vector vec) {
        for (auto lhsIt = vec.data.begin(); lhsIt != vec.data.end(); ++lhsIt)
            *lhsIt /= scalar;
        return vec;
    }

    // Output stream operator
    friend std::ostream& operator<<(std::ostream& os, const Vector& vec) {
        os << "[";
        for (auto& i : vec.data)
            os << i << ", ";
        return os << "]";
    }

    // Static methods
    static Vector Normalized(Vector vec) {
        return vec.Normalize();
    }

    static Vector Inverted(Vector vec) {
        return vec.Invert();
    }

    static float Dot(const Vector& lhs, const Vector& rhs) {
        auto ret = 0.f;
        auto rhsIt = rhs.data.begin();
        for (auto lhsIt = lhs.data.begin(); lhsIt != lhs.data.end(); ++lhsIt, ++rhsIt)
            ret += *lhsIt * *rhsIt;
        return ret;
    }

    static Vector Cross(const Vector& lhs, const Vector& rhs) {
        return { lhs.y() * rhs.z() - rhs.y() * lhs.z(), lhs.z() * rhs.x() - rhs.z() * lhs.x(), lhs.x() * rhs.y() - rhs.x() * lhs.y() };
    }

    inline static float Distance(const Vector& lhs, const Vector& rhs) {
        return (lhs - rhs).Magnitude();
    }

    Vector<2> To2() { return { data[0], data[2] }; }
    Vector<3> To3() { return { data[0], data[1], data[2] }; }
    Vector<4> To4() { return { data[0], data[1], data[2], data[3] }; }
};

template <>
inline Vector<3> Vector<2>::To3() {
    return { data[0], 0.f, data[1] };
}

template <>
inline Vector<4> Vector<2>::To4() {
    return { data[0], 0.f, data[2], 1.f };
}

template <>
inline Vector<4> Vector<3>::To4() {
    return { data[0], data[1], data[2], 1.f };
}

} // namespace Geometry