#pragma once

#include "Collider.hpp"

namespace Collisions {

class BoundsCollider : public Collider {
public:
    const float radius;

    BoundsCollider(float radius)
        : radius(radius) {}

    // Visitors
    void visit(ColliderVisitor& visitor) { visitor(*this); }
    void visit(ConstColliderVisitor& visitor) const { visitor(*this); }
};

} // namespace Collisions