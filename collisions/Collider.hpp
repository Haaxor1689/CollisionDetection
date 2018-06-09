#pragma once

#include "ColliderVisitor.hpp"

namespace Collisions {

struct Collider {
    virtual ~Collider() = default;

    virtual void Visit(ColliderVisitor& visitor) = 0;
    virtual void Visit(ConstColliderVisitor& visitor) const = 0;
};

} // namespace Collisions