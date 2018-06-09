#pragma once

#include <vector>

#include "Geometry"

const unsigned SEGMENTS = 36;
const float RADIUS = 40.f;

const float ANGLE = -2 * Geometry::pi / float(SEGMENTS);
const float S = sinf(ANGLE);
const float C = cosf(ANGLE);

const float BRICK_WIDTH = 3.f;
const float BRICK_HEIGHT = 3.f;

inline void EmplaceVert(std::vector<float>& vec, float x, float y, float z, float nx, float ny, float nz, float u, float v) {
    vec.emplace_back(x);
    vec.emplace_back(y);
    vec.emplace_back(z);
    vec.emplace_back(nx);
    vec.emplace_back(ny);
    vec.emplace_back(nz);
    vec.emplace_back(u);
    vec.emplace_back(v);
}

inline void EmplaceFiller(std::vector<float>& vec, float x, float y, float z) {
    EmplaceVert(vec, x, y, z, 0.f, 0.f, 0.f, 0.f, 0.f);
}

inline void Rotate(float& x, float& z) {
    const auto newX = x * C - z * S;
    const auto newZ = x * S + z * C;
    x = newX;
    z = newZ;
}

inline std::vector<float> GetGroundVertices() {
    std::vector<float> ret;
    ret.reserve((SEGMENTS + 2) * 5);

    // Center
    EmplaceVert(ret, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f);

    float x = RADIUS;
    float z = 0.f;
    for (unsigned j = 0; j <= SEGMENTS; ++j) {
        EmplaceVert(ret, x, 0.f, z, 0.f, 1.f, 0.f, x, z);
        Rotate(x, z);
    }
    return ret;
}

inline std::vector<float> GetBrickVertices(float distance, unsigned segmentsCount) {
    std::vector<float> ret;
    ret.reserve(segmentsCount * 20);

    // Front
    float x = distance;
    float z = 0.f;
    for (unsigned j = 0; j <= segmentsCount; ++j) {
        EmplaceVert(ret, x, 0, z, -x, 0.f, -z, j, 1.f);
        EmplaceVert(ret, x, BRICK_HEIGHT, z, -x, 0.f, -z, j, 0.f);
        if (j != segmentsCount) {
            Rotate(x, z);
        }
    }
    EmplaceFiller(ret, x, BRICK_HEIGHT, z);

    // Back
    x = distance + BRICK_WIDTH;
    z = 0.f;

    EmplaceFiller(ret, x, BRICK_HEIGHT, z);
    for (unsigned j = 0; j <= segmentsCount; ++j) {
        EmplaceVert(ret, x, BRICK_HEIGHT, z, x, 0.f, z, j, 1.f);
        EmplaceVert(ret, x, 0, z, x, 0.f, z, j, 2.f);
        if (j != segmentsCount) {
            Rotate(x, z);
        }
    }
    EmplaceFiller(ret, x, 0, z);

    // Top
    float x1 = distance;
    float z1 = 0.f;
    float x2 = distance + BRICK_WIDTH;
    float z2 = 0.f;

    EmplaceFiller(ret, x1, BRICK_HEIGHT, z1);
    for (unsigned j = 0; j <= segmentsCount; ++j) {
        EmplaceVert(ret, x1, BRICK_HEIGHT, z1, 0.f, 1.f, 0.f, j, 0.f);
        EmplaceVert(ret, x2, BRICK_HEIGHT, z2, 0.f, 1.f, 0.f, j, 1.f);
        if (j != segmentsCount) {
            Rotate(x1, z1);
            Rotate(x2, z2);
        }
    }
    EmplaceFiller(ret, x2, BRICK_HEIGHT, z2);

    // Caps
    EmplaceVert(ret, x2, BRICK_HEIGHT, z2, z2, 0.f, -x2, 0.f, 0.f);
    EmplaceVert(ret, x1, BRICK_HEIGHT, z1, z2, 0.f, -x2, 1.f, 0.f);
    EmplaceVert(ret, x2, 0.f, z2, z2, 0.f, -x2, 0.f, 1.f);
    EmplaceVert(ret, x1, 0.f, z1, z2, 0.f, -x2, 1.f, 1.f);
    EmplaceFiller(ret, x1, 0.f, z1);

    EmplaceFiller(ret, distance + BRICK_WIDTH, 0.f, 0.f);
    EmplaceVert(ret, distance + BRICK_WIDTH, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f);
    EmplaceVert(ret, distance, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f);
    EmplaceVert(ret, distance + BRICK_WIDTH, BRICK_HEIGHT, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f);
    EmplaceVert(ret, distance, BRICK_HEIGHT, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f);
    return ret;
}

const float PAD_DISTANCE = RADIUS * 3.f / 4.f;
const float PAD_SEGMENTS = SEGMENTS / 6.f;
const float BRICK_DISTANCE = RADIUS / 4.f;
const float BRICK_SEGMENTS = SEGMENTS / 12.f;

const std::vector<float> GROUND_VERTICES = GetGroundVertices();
const std::vector<float> PAD_VERTICES = GetBrickVertices(PAD_DISTANCE, PAD_SEGMENTS);
const std::vector<float> BRICK_VERTICES = GetBrickVertices(BRICK_DISTANCE, BRICK_SEGMENTS);