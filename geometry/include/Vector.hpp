#pragma once

#include <array>
#include <numeric>
#include <cmath>
#include <ostream>
#include <stdexcept>

namespace Geometry {
class Vector {
    std::array<float, 3> data;

public:
    Vector()
        : data{ 0.f, 0.f, 0.f } {};
    Vector(float x, float y, float z)
        : data{ x, y, z } {}
    Vector(float x)
        : Vector(x, x, x) {}

    float Magnitude() const {
        return std::sqrt(x() * x() + y() * y() + z() * z());
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

    Vector& Rotate(float angle, Vector axis);

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

        x() /= mag;
        y() /= mag;
        z() /= mag;
        return *this;
    }

    Vector& Invert() {
        x() *= -1.f;
        y() *= -1.f;
        z() *= -1.f;
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
        return os << "[" << vec.x() << ", " << vec.y() << ", " << vec.z() << "]";
    }

    // Static methods
    static Vector Normalized(Vector vec) {
        return vec.Normalize();
    }

    static Vector Inverted(Vector vec) {
        return vec.Invert();
    }

    static float Dot(const Vector& lhs, const Vector& rhs) {
        return lhs.x() * rhs.x() + lhs.y() * rhs.y() + lhs.z() * rhs.z();
    }

    static Vector Cross(const Vector& lhs, const Vector& rhs) {
        return { lhs.y() * rhs.z() - rhs.y() * lhs.z(), lhs.z() * rhs.x() - rhs.z() * lhs.x(), lhs.x() * rhs.y() - rhs.x() * lhs.y() };
    }

    inline static float Distance(const Vector& lhs, const Vector& rhs) {
        return (lhs - rhs).Magnitude();
    }
};
} // namespace Geometry