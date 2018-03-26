#pragma once

#include "Vector.hpp"

namespace Geometry {
class Ray {
    Vector start;
    Vector direction;

public:
    Ray(const Vector& start, const Vector& direction)
        : start(start), direction(direction) {}

    float DistanceTo(const Ray& other) {
        throw;
    }

    float DistanceTo(const Vector& position) {
        throw;
    }
};
} // namespace Geometry