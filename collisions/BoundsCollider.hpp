#pragma once

#include "Collider.hpp"

namespace Collisions {

struct BoundsCollider : public Collider {
    const float radius;

    BoundsCollider(float radius)
        : radius(radius) {}

    // Visitors
    void visit(ColliderVisitor& visitor) { visitor(*this); }
    void visit(ConstColliderVisitor& visitor) const { visitor(*this); }
};

} // namespace Collisions