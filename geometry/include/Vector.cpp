#include "Vector.hpp"

#include "Matrix.hpp"

using namespace Geometry;

Vector& Vector::Rotate(float angle, Vector axis) {
    float const c = cos(angle);
    float const s = sin(angle);

    axis.Normalize();
    Vector temp = axis * (1.f - c);

    Matrix<3> rotation;
    rotation[0][0] = c + temp[0] * axis[0];
    rotation[1][0] = temp[0] * axis[1] + s * axis[2];
    rotation[2][0] = temp[0] * axis[2] - s * axis[1];

    rotation[0][1] = temp[1] * axis[0] - s * axis[2];
    rotation[1][1] = c + temp[1] * axis[1];
    rotation[2][1] = temp[1] * axis[2] + s * axis[0];

    rotation[0][2] = temp[2] * axis[0] + s * axis[1];
    rotation[1][2] = temp[2] * axis[1] - s * axis[0];
    rotation[2][2] = c + temp[2] * axis[2];

    Vector result;
    result[0] = (*this)[0] * rotation[0][0] + (*this)[1] * rotation[0][1] + (*this)[2] * rotation[0][2];
    result[1] = (*this)[0] * rotation[1][0] + (*this)[1] * rotation[1][1] + (*this)[2] * rotation[1][2];
    result[2] = (*this)[0] * rotation[2][0] + (*this)[1] * rotation[2][1] + (*this)[2] * rotation[2][2];
    return *this = result;
}