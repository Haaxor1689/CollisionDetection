#pragma once

#include "Collider.hpp"
#include "objects.inl"

namespace Collisions {

class BrickCollider : public Collider {
    float distance;
    unsigned segmentsCount;
    float angleStart;
    float height;

public:
    BrickCollider(float distance, unsigned segmentsCount, float angle = 0.f, float height = 0.f)
        : distance(distance), segmentsCount(segmentsCount), angleStart(angle), height(height) {}

    // Visitors
    void visit(ColliderVisitor& visitor) { visitor(*this); }
    void visit(ConstColliderVisitor& visitor) const { visitor(*this); }

    void Rotate(float angle) {
        angleStart += angle;
        if (angleStart > Geometry::Pi) {
            angleStart -= 2 * Geometry::Pi;
        } else if (angleStart < -Geometry::Pi) {
            angleStart += 2 * Geometry::Pi;
        }
    }

    float InnerRadius() const { return distance; }
    float OuterRadius() const { return distance + brick_width; }
    float MiddleRadius() const { return distance + brick_width / 2.f; }
    float AngleStart() const { return angleStart; }
    float AngleEnd() const { return angleStart + static_cast<float>(segmentsCount) * angle; }
    float Height() const { return height; }
};

} // namespace Collisions