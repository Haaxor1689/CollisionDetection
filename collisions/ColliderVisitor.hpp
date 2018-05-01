#pragma once

namespace Collisions {

class BallCollider;
class BoundsCollider;
class BrickCollider;

class IColliderVisitor {
public:
    virtual ~IColliderVisitor() = default;

    virtual void operator()(BallCollider&) = 0;
    virtual void operator()(BoundsCollider&) = 0;
    virtual void operator()(BrickCollider&) = 0;
};

class IConstColliderVisitor {
public:
    virtual ~IConstColliderVisitor() = default;

    virtual void operator()(const BallCollider&) = 0;
    virtual void operator()(const BoundsCollider&) = 0;
    virtual void operator()(const BrickCollider&) = 0;
};

class ColliderVisitor : IColliderVisitor {
public:
    virtual ~ColliderVisitor() = default;

    virtual void operator()(BallCollider&) override{};
    virtual void operator()(BoundsCollider&) override{};
    virtual void operator()(BrickCollider&) override{};
};

class ConstColliderVisitor : IConstColliderVisitor {
public:
    virtual ~ConstColliderVisitor() = default;

    virtual void operator()(const BallCollider&) override{};
    virtual void operator()(const BoundsCollider&) override{};
    virtual void operator()(const BrickCollider&) override{};
};

} // namespace Collisions