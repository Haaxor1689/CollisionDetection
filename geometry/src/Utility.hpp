#pragma once

#include <math.h>

namespace Geometry {
constexpr float Pi() {
    return std::atan(1) * 4;
}

constexpr float Radians(float degrees) {
    return degrees * Pi() / 180;
}
} // namespace Geometry