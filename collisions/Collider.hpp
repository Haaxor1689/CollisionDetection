#pragma once

#include "ColliderVisitor.hpp"

namespace Collisions {

struct Collider {
    virtual ~Collider() = default;

    virtual void visit(ColliderVisitor& visitor) = 0;
    virtual void visit(ConstColliderVisitor& visitor) const = 0;
};

} // namespace Collisions