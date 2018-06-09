#pragma once

#include "Vector.hpp"
#include "Matrix.hpp"
#include <iomanip>
#include <sstream>

namespace Geometry {

constexpr float pi = 3.14159265358979323846f;

inline std::string ToString(float value, int precision) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << value;
    return stream.str();
}

constexpr float Radians(float degrees) {
    return degrees * pi / 180;
}

constexpr float Degrees(float radians) {
    return radians * 180 / pi;
}

inline Matrix<4> Perspective(float fov, float aspect, float nearPlane, float farPlane) {
    const auto s = 1.f / std::tan(Radians(fov) * 0.5f);

    Matrix<4> ret(0.f);
    ret[0][0] = 1.f / aspect * s;
    ret[1][1] = s;
    ret[3][2] = -1.f;
    ret[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
    ret[2][3] = -(2.f * farPlane * nearPlane) / (farPlane - nearPlane);
    return ret;
}

inline Matrix<4> LookAt(Vector<3> eye, Vector<3> center, Vector<3> up) {
    auto f = Vector<3>::Normalized(center - eye);
    auto s = Vector<3>::Normalized(Vector<3>::Cross(f, up));
    auto u = Vector<3>::Cross(s, f);

    auto ret = Matrix<4>::Identity();
    ret[0][0] = s.X();
    ret[0][1] = s.Y();
    ret[0][2] = s.Z();
    ret[1][0] = u.X();
    ret[1][1] = u.Y();
    ret[1][2] = u.Z();
    ret[2][0] = -f.X();
    ret[2][1] = -f.Y();
    ret[2][2] = -f.Z();
    ret[0][3] = -Vector<3>::Dot(s, eye);
    ret[1][3] = -Vector<3>::Dot(u, eye);
    ret[2][3] = Vector<3>::Dot(f, eye);
    return ret;
}

} // namespace Geometry
