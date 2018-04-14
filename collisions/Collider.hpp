#pragma once

#include <variant>
#include <optional>

#include "Geometry"

// Helper class for variant visiting
template <class... Ts>
struct Visitor : Ts... { using Ts::operator()...; };
template <class... Ts>
Visitor(Ts...)->Visitor<Ts...>;

namespace Collisions {
struct PadCollider {
};

struct BoundsCollider {
};

struct BallCollider {
    Geometry::Vector position;
    Geometry::Vector velocity;
    float radius;

    bool DidCollideWith(const std::variant<BallCollider, PadCollider, BoundsCollider>& other) const {
        return std::visit(Visitor{
                              [this](const auto& arg) { return false; },
                              [this](const BallCollider& arg) { return DidCollide(arg); },
                              [this](const PadCollider& arg) { return DidCollide(arg); },
                              [this](const BoundsCollider& arg) { return DidCollide(arg); },
                          },
                          other);
    }

    bool DidCollide(const BallCollider& other) const {
        return (position - other.position).Magnitude() < radius + other.radius;
    }

    bool DidCollide(const PadCollider& other) const {
        return false;
    }

    bool DidCollide(const BoundsCollider& other) const {
        return false;
    }

    Geometry::Vector Collision(const BallCollider& other) {
        return Geometry::Vector();
    }

    Geometry::Vector Collision(const PadCollider& other) {
        return Geometry::Vector();
    }

    Geometry::Vector Collision(const BoundsCollider& other) {
        return Geometry::Vector();
    }
};
} // namespace Collisions