#pragma once

#include "Vector.hpp"

namespace Geometry {
class Ray {
    Vector<3> start;
    Vector<3> direction;

public:
    Ray(const Vector<3>& start, const Vector<3>& direction)
        : start(start), direction(direction) {}

    float DistanceTo(const Ray& other) {
        throw;
    }

    float DistanceTo(const Vector<3>& position) {
        throw;
    }
};
} // namespace Geometry