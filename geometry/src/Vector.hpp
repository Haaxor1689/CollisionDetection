#pragma once

#include <cmath>
#include <ostream>
#include <stdexcept>

namespace Geometry {

class Vector {
public:
    float x, y, z;

    Vector()
        : x(0), y(0), z(0) {}
    Vector(float x, float y, float z)
        : x(x), y(y), z(z) {}
    Vector(float x)
        : Vector(x, x, x) {}

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
        x *= -1.f;
        y *= -1.f;
        z *= -1.f;
        return *this;
    }

    // Bool operators
    friend bool operator==(const Vector& lhs, const Vector& rhs) { return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z; }
    friend bool operator!=(const Vector& lhs, const Vector& rhs) { return !(lhs == rhs); }
    // Vector arithmetic operators
    friend Vector operator+(const Vector& lhs, const Vector& rhs) { return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z }; }
    friend Vector operator-(const Vector& lhs, const Vector& rhs) { return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z }; }
    friend Vector operator*(const Vector& lhs, const Vector& rhs) { return { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z }; }
    friend Vector operator/(const Vector& lhs, const Vector& rhs) { return { lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z }; }
    // Scalar arithmetic operators
    friend Vector operator+(const Vector& vec, float scalar) { return { vec.x + scalar, vec.y + scalar, vec.z + scalar }; }
    friend Vector operator-(const Vector& vec, float scalar) { return { vec.x - scalar, vec.y - scalar, vec.z - scalar }; }
    friend Vector operator*(const Vector& vec, float scalar) { return { vec.x * scalar, vec.y * scalar, vec.z * scalar }; }
    friend Vector operator/(const Vector& vec, float scalar) { return { vec.x / scalar, vec.y / scalar, vec.z / scalar }; }
    friend Vector operator+(float scalar, const Vector& vec) { return { vec.x + scalar, vec.y + scalar, vec.z + scalar }; }
    friend Vector operator-(float scalar, const Vector& vec) { return { vec.x - scalar, vec.y - scalar, vec.z - scalar }; }
    friend Vector operator*(float scalar, const Vector& vec) { return { vec.x * scalar, vec.y * scalar, vec.z * scalar }; }
    friend Vector operator/(float scalar, const Vector& vec) { return { vec.x / scalar, vec.y / scalar, vec.z / scalar }; }
    // Output stream operator
    friend std::ostream& operator<<(std::ostream& os, const Vector& vec) {
        return os << "[" << vec.x << ", " << vec.y << ", " << vec.z << "]";
    }

    // Static methods
    static Vector Normalized(Vector vec) {
        return vec.Normalize();
    }

    static Vector Inverted(Vector vec) {
        return vec.Invert();
    }

    static float Dot(const Vector& lhs, const Vector& rhs) {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    }

    static Vector Cross(const Vector& lhs, const Vector& rhs) {
        return { lhs.y * rhs.z - rhs.y * lhs.z, lhs.z * rhs.x - rhs.z * lhs.x, lhs.x * rhs.y - rhs.x * lhs.y };
    }
};

} // namespace Geometry