#pragma once

#include "objects.inl"
#include "Geometry"

namespace Collisions {
class BrickCollider {
    float innerRadius;
    float outerRadius;
    Geometry::Vector startPoint;
    Geometry::Vector endPoint;

public:
    BrickCollider(float distance, unsigned segmentsCount)
        : innerRadius(distance), outerRadius(distance + brick_width), startPoint(distance, 0.f, 0.f) {
        float x = distance;
        float z = 0.f;
        for (unsigned i = 0; i < segmentsCount; ++i)
            rotate(x, z);
        endPoint = { x, 0.f, z };
    }

    float InnerRadius() const { return innerRadius; }
    float OuterRadius() const { return outerRadius; }
    const Geometry::Vector& StartPoint() const { return startPoint; }
    const Geometry::Vector& EndPoint() const { return endPoint; }
};

struct BoundsCollider {
    const float radius;
};

class BallCollider {
    Geometry::Vector position;
    Geometry::Vector velocity;

public:
    const float radius;

    BallCollider(Geometry::Vector&& position, Geometry::Vector&& velocity, float radius)
        : position(std::move(position)), velocity(std::move(velocity)), radius(radius) {}

    bool DidCollide(const BallCollider& other) const {
        return Geometry::Vector::Distance(position, other.position) < radius + other.radius;
    }

    bool DidCollide(const BrickCollider& other) const {
        float mag = position.Magnitude();
        if (mag < other.InnerRadius() - radius ||
            mag > other.OuterRadius() + radius) {
            return false;
        }

        auto dot1 = Geometry::Vector::Dot(position, other.StartPoint());
        auto projected1 = dot1 / Geometry::Vector::Dot(other.StartPoint(), other.StartPoint()) * other.StartPoint();
        auto distance1 = Geometry::Vector::Distance(position, projected1);

        auto side1 = position.x() * other.StartPoint().z() - position.z() * other.StartPoint().x();

        auto dot2 = Geometry::Vector::Dot(position, other.EndPoint());
        auto projected2 = dot2 / Geometry::Vector::Dot(other.EndPoint(), other.EndPoint()) * other.EndPoint();
        auto distance2 = Geometry::Vector::Distance(position, projected2);

        auto side2 = position.x() * other.EndPoint().z() - position.z() * other.EndPoint().x();

        return distance1 < radius || distance2 < radius || (side1 > 0.f && side2 < 0.f);
    }

    bool DidCollide(const BoundsCollider& other) const {
        return position.Magnitude() > other.radius - radius;
    }

    Geometry::Vector Collision(const BallCollider& other) {
        return Geometry::Vector();
    }

    Geometry::Vector Collision(const BrickCollider& other) {
        return Geometry::Vector();
    }

    Geometry::Vector Collision(const BoundsCollider& other) {
        return Geometry::Vector();
    }
};
} // namespace Collisions