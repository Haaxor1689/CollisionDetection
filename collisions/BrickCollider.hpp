#pragma once

#include <iostream>

#include "Collider.hpp"
#include "objects.inl"

namespace Collisions {

class BrickCollider : public Collider {
    float distance;
    unsigned segmentsCount;
    float angleStart;
    float height;
    float angularVelocity = 0.f;

public:
    BrickCollider(float distance, unsigned segmentsCount, float angle = 0.f, float height = 0.f)
        : distance(distance),
          segmentsCount(segmentsCount),
          angleStart(angle > Geometry::pi ? angle - 2 * Geometry::pi : angle < -Geometry::pi ? angle + 2 * Geometry::pi : angle),
          height(height) {}

    // Visitors
    void Visit(ColliderVisitor& visitor) override { visitor(*this); }
    void Visit(ConstColliderVisitor& visitor) const override { visitor(*this); }

    void Rotate(float angle) {
        angularVelocity = angle;
        angleStart += angle;
        if (angleStart > Geometry::pi) {
            angleStart -= 2 * Geometry::pi;
        } else if (angleStart < -Geometry::pi) {
            angleStart += 2 * Geometry::pi;
        }
    }

    float InnerRadius() const { return distance; }
    float OuterRadius() const { return distance + BRICK_WIDTH; }
    float MiddleRadius() const { return distance + BRICK_WIDTH / 2.f; }
    float AngleStart() const { return angleStart; }
    float AngleEnd() const { return angleStart + static_cast<float>(segmentsCount) * ANGLE; }
    float Height() const { return height; }

    Geometry::Vector<3> Velocity(const Geometry::Vector<3>& position) const {
        const auto rotated = Geometry::Vector<3>(position).Rotate(angularVelocity, { 0.f, 1.f, 0.f });
        return (rotated - position).Invert();
    }

    Geometry::Vector<3> InnerStartCorner() const { return Geometry::Vector<3>{ InnerRadius(), 1.f, 0.f }.Rotate(-AngleStart(), { 0.f, 1.f, 0.f }); }
    Geometry::Vector<3> InnerEndCorner() const { return Geometry::Vector<3>{ InnerRadius(), 1.f, 0.f }.Rotate(-AngleEnd(), { 0.f, 1.f, 0.f }); }
    Geometry::Vector<3> OuterStartCorner() const { return Geometry::Vector<3>{ OuterRadius(), 1.f, 0.f }.Rotate(-AngleStart(), { 0.f, 1.f, 0.f }); }
    Geometry::Vector<3> OuterEndCorner() const { return Geometry::Vector<3>{ OuterRadius(), 1.f, 0.f }.Rotate(-AngleEnd(), { 0.f, 1.f, 0.f }); }
};

} // namespace Collisions
