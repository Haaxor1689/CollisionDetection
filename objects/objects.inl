#pragma once

#include <vector>
#include <cmath>
#include <math.h>

#include "Geometry"

const unsigned segments = 36;
const float radius = 5.f;

const float angle = -2 * Geometry::Pi() / float(segments);
const float s = sinf(angle);
const float c = cosf(angle);

inline void emplace_vert(std::vector<float>& vec, float x, float y, float z, float nx, float ny, float nz, float u, float v) {
    vec.emplace_back(x);
    vec.emplace_back(y);
    vec.emplace_back(z);
    vec.emplace_back(nx);
    vec.emplace_back(ny);
    vec.emplace_back(nz);
    vec.emplace_back(u);
    vec.emplace_back(v);
}

inline void emplace_filler(std::vector<float>& vec, float x, float y, float z) {
    emplace_vert(vec, x, y, z, 0.f, 0.f, 0.f, 0.f, 0.f);
}

inline void rotate(float& x, float& z) {
    float newX = x * c - z * s;
    float newZ = x * s + z * c;
    x = newX;
    z = newZ;
}

std::vector<float> get_ground_vertices() {
    std::vector<float> ret;
    ret.reserve((segments + 2) * 5);

    // Center
    emplace_vert(ret, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f);

    float x = radius;
    float z = 0.f;
    for (unsigned j = 0; j <= segments; ++j) {
        emplace_vert(ret, x, 0.f, z, 0.f, 1.f, 0.f, x, z);
        rotate(x, z);
    }
    return ret;
};

std::vector<float> get_brick_vertices(float distance, unsigned segmentsCount) {
    std::vector<float> ret;
    ret.reserve(segmentsCount * 20);

    const float width = 0.5f;
    const float height = 0.5f;

    // Front
    float x = distance;
    float z = 0.f;
    for (unsigned j = 0; j <= segmentsCount; ++j) {
        emplace_vert(ret, x, 0, z, -x, 0.f, -z, j, 1.f);
        emplace_vert(ret, x, height, z, -x, 0.f, -z, j, 0.f);
        if (j != segmentsCount) {
            rotate(x, z);
        }
    }
    emplace_filler(ret, x, height, z);

    // Back
    x = distance + width;
    z = 0.f;

    emplace_filler(ret, x, height, z);
    for (unsigned j = 0; j <= segmentsCount; ++j) {
        emplace_vert(ret, x, height, z, x, 0.f, z, j, 1.f);
        emplace_vert(ret, x, 0, z, x, 0.f, z, j, 2.f);
        if (j != segmentsCount) {
            rotate(x, z);
        }
    }
    emplace_filler(ret, x, 0, z);

    // Top
    float x1 = distance;
    float z1 = 0.f;
    float x2 = distance + width;
    float z2 = 0.f;

    emplace_filler(ret, x1, height, z1);
    for (unsigned j = 0; j <= segmentsCount; ++j) {
        emplace_vert(ret, x1, height, z1, 0.f, 1.f, 0.f, j, 0.f);
        emplace_vert(ret, x2, height, z2, 0.f, 1.f, 0.f, j, 1.f);
        if (j != segmentsCount) {
            rotate(x1, z1);
            rotate(x2, z2);
        }
    }
    emplace_filler(ret, x2, height, z2);

    // Caps
    emplace_vert(ret, x2, height, z2, z2, 0.f, -x2, 0.f, 0.f);
    emplace_vert(ret, x1, height, z1, z2, 0.f, -x2, 1.f, 0.f);
    emplace_vert(ret, x2, 0.f, z2, z2, 0.f, -x2, 0.f, 1.f);
    emplace_vert(ret, x1, 0.f, z1, z2, 0.f, -x2, 1.f, 1.f);
    emplace_filler(ret, x1, 0.f, z1);

    emplace_filler(ret, distance + width, 0.f, 0.f);
    emplace_vert(ret, distance + width, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f);
    emplace_vert(ret, distance, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f);
    emplace_vert(ret, distance + width, height, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f);
    emplace_vert(ret, distance, height, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f);
    return ret;
};

const std::vector<float> ground_vertices = get_ground_vertices();
const std::vector<float> pad_vertices = get_brick_vertices(radius * 3.f / 4.f, segments / 5.f);
const std::vector<float> brick_vertices = get_brick_vertices(radius / 4.f, segments / 2.f);