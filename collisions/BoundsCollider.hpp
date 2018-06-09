#pragma once

#include "Collider.hpp"

namespace Collisions {

class BoundsCollider : public Collider {
public:
    const float radius;

    BoundsCollider(float radius)
        : radius(radius) {}

    // Visitors
    void Visit(ColliderVisitor& visitor) override { visitor(*this); }
    void Visit(ConstColliderVisitor& visitor) const override { visitor(*this); }
};

} // namespace Collisions