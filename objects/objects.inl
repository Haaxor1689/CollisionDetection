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

inline void emplace_up_normal(std::vector<float>& ret) {
    ret.emplace_back(0.f);
    ret.emplace_back(1.f);
    ret.emplace_back(0.f);
};

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
    ret.emplace_back(0.f);
    ret.emplace_back(0.f);
    ret.emplace_back(0.f);

    emplace_up_normal(ret);

    ret.emplace_back(0.f);
    ret.emplace_back(0.f);

    float x = radius;
    float z = 0.f;
    for (unsigned j = 0; j <= segments; ++j) {

        // Vertices
        ret.emplace_back(x);
        ret.emplace_back(0.f);
        ret.emplace_back(z);

        // Normals
        emplace_up_normal(ret);

        // Tex coords
        ret.emplace_back(x);
        ret.emplace_back(z);

        rotate(x, z);
    }
    return ret;
};

std::vector<float> get_brick_vertices(float distance, unsigned segmentsCount) {
    std::vector<float> ret;
    ret.reserve(segmentsCount * 20);

    const float width = 0.5f;
    const float height = 0.5f;

    float x1 = distance;
    float z1 = 0.f;
    float x2 = distance + width;
    float z2 = 0.f;
    for (unsigned j = 0; j <= segmentsCount; ++j) {
        for (float y : { 0.f, height }) {
            // Vertices
            ret.emplace_back(x1);
            ret.emplace_back(y);
            ret.emplace_back(z1);

            // Normals
            emplace_up_normal(ret);

            // Tex coords
            ret.emplace_back(j);
            ret.emplace_back(y == 0.f ? 1 : 0);

            // Vertices
            ret.emplace_back(x2);
            ret.emplace_back(y);
            ret.emplace_back(z2);

            // Normals
            emplace_up_normal(ret);

            // Tex coords
            ret.emplace_back(j);
            ret.emplace_back(y == 0.f ? 2 : 1);
        }

        rotate(x1, z1);
        rotate(x2, z2);
    }
    return ret;
};

std::vector<uint32_t> get_brick_indices(unsigned segmentsCount) {
    std::vector<uint32_t> ret;
    ret.reserve(segmentsCount * 6 + 2);

    uint32_t index = 0;
    for (unsigned i = 0; i <= segmentsCount; ++i) {
        ret.emplace_back(index);
        index += 2;
        ret.emplace_back(index);
        index += 2;
    }
    index -= 3;
    for (unsigned i = 0; i <= segmentsCount; ++i) {
        ret.emplace_back(index);
        index += 2;
        ret.emplace_back(index);
        index -= 6;
    }
    ret.emplace_back(0);
    ret.emplace_back(2);
    index = 2;
    for (unsigned i = 0; i <= segmentsCount; ++i) {
        ret.emplace_back(index++);
        ret.emplace_back(index);
        index += 3;
    }

    return ret;
};

const std::vector<float> ground_vertices = get_ground_vertices();

const unsigned padSegmentsCount = segments / 5.f;
const std::vector<float> pad_vertices = get_brick_vertices(radius * 3.f / 4.f, padSegmentsCount);
const std::vector<uint32_t> pad_indices = get_brick_indices(padSegmentsCount);

const unsigned brickSegmentsCount = segments / 2.f;
const std::vector<float> brick_vertices = get_brick_vertices(radius / 4.f, brickSegmentsCount);
const std::vector<uint32_t> brick_indices = get_brick_indices(brickSegmentsCount);