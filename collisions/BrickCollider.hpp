#pragma once

#include "Collider.hpp"
#include "objects.inl"

namespace Collisions {

class BrickCollider : public Collider {
    float innerRadius;
    float outerRadius;
    Geometry::Vector<3> startPoint;
    Geometry::Vector<3> endPoint;

    float angle;
    float height;

public:
    BrickCollider(float distance, unsigned segmentsCount, float angle = 0.f, float height = 0.f)
        : innerRadius(distance), outerRadius{ distance + brick_width }, startPoint{ distance, 0.f, 0.f }, angle(angle), height(height) {
        float x = distance;
        float z = 0.f;
        for (unsigned i = 0; i < segmentsCount; ++i)
            rotate(x, z);
        endPoint = { x, 0.f, z };
    }

    // Visitors
    void visit(ColliderVisitor& visitor) {
        visitor(*this);
    }
    void visit(ConstColliderVisitor& visitor) const {
        visitor(*this);
    }

    void Rotate(float angle) {
        this->angle += angle;
    }

    float InnerRadius() const {
        return innerRadius;
    }
    float OuterRadius() const {
        return outerRadius;
    }
    const Geometry::Vector<3>& StartPoint() const {
        return startPoint;
    }
    const Geometry::Vector<3>& EndPoint() const {
        return endPoint;
    }
    float Angle() const {
        return angle;
    }
    float Height() const {
        return height;
    }
};

} // namespace Collisions