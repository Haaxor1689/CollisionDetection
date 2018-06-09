#pragma once

#include "Vector.hpp"
#include "Matrix.hpp"

namespace Geometry {
constexpr float Pi = 3.14159265358979323846f;

constexpr float Radians(float degrees) {
    return degrees * Pi / 180;
}

constexpr float Degrees(float radians) {
    return radians * 180 / Pi;
}

inline Matrix<4> Perspective(float fov, float aspect, float near_, float far_) {
    float S = 1.f / std::tan(Radians(fov) * 0.5f);

    Matrix<4> ret(0.f);
    ret[0][0] = 1.f / aspect * S;
    ret[1][1] = S;
    ret[3][2] = -1.f;
    ret[2][2] = -(far_ + near_) / (far_ - near_);
    ret[2][3] = -(2.f * far_ * near_) / (far_ - near_);
    return ret;
}

inline Matrix<4> LookAt(Vector<3> eye, Vector<3> center, Vector<3> up) {
    auto f = Vector<3>::Normalized(center - eye);
    auto s = Vector<3>::Normalized(Vector<3>::Cross(f, up));
    auto u = Vector<3>::Cross(s, f);

    auto ret = Matrix<4>::Identity();
    ret[0][0] = s.x();
    ret[0][1] = s.y();
    ret[0][2] = s.z();
    ret[1][0] = u.x();
    ret[1][1] = u.y();
    ret[1][2] = u.z();
    ret[2][0] = -f.x();
    ret[2][1] = -f.y();
    ret[2][2] = -f.z();
    ret[0][3] = -Vector<3>::Dot(s, eye);
    ret[1][3] = -Vector<3>::Dot(u, eye);
    ret[2][3] = Vector<3>::Dot(f, eye);
    return ret;
}
} // namespace Geometry