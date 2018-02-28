#pragma once

#include <cmath>
#include <ostream>

namespace MyEngine {

class Vector {
public:
    float x, y, z;

    Vector() : x(0), y(0), z(0) {}
    Vector(float x, float y, float z) : x(x), y(y), z(z) {}

    float Magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vector& Normalize() {
        float mag = Magnitude();
        if (mag == 0)
            return *this;

        x /= mag;
        y /= mag;
        z /= mag;
        return *this;
    }

    Vector& Invert() {
        return *this;
    }

    // Operators
    friend bool operator==(const Vector& lhs, const Vector& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
    }

    friend bool operator!=(const Vector& lhs, const Vector& rhs) {
        return !(lhs == rhs);
    }

    friend Vector operator+(const Vector& lhs, const Vector& rhs) {
        return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
    }

    friend Vector operator-(const Vector& lhs, const Vector& rhs) {
        return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector& vec) {
        return os << "[" << vec.x << ", " << vec.y << ", " << vec.z << "]";
    }

    // Static methods
    static Vector Normalized(const Vector& vec) {
        return Vector(vec).Normalize();
    }

    // static Vector Inverted(const Vector& vec) {
    //     return vec;
    // }

    // static float Dot(const Vector& lhs, const Vector& rhs) {

    // }

    // static Vector Cross(const Vector& lhs, const Vector& rhs) {

    // }
};

}