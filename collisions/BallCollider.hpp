#pragma once

#include "Collider.hpp"
#include "BoundsCollider.hpp"
#include "BrickCollider.hpp"
#include "objects.inl"
#include <iostream>

namespace Collisions {

class BallCollider : public Collider {
    Geometry::Vector<3> position;
    Geometry::Vector<3> velocity;

public:
    const float Radius;
    const float Mass;
    const float MaxVelocity;

    BallCollider(Geometry::Vector<3>&& position, Geometry::Vector<3>&& velocity, float radius, float maxVelocity = 1.5f)
        : position(std::move(position)),
          velocity(std::move(velocity)),
          Radius(radius),
          Mass(4 * Geometry::pi * radius * radius * radius * 11.34f / 3),
          MaxVelocity(maxVelocity) {}

    // Visitors
    void Visit(ColliderVisitor& visitor) override { visitor(*this); }
    void Visit(ConstColliderVisitor& visitor) const override { visitor(*this); }

    void Step() {
        position += velocity;

        if (velocity.Magnitude() > MaxVelocity) {
            velocity *= 0.9;
        }
    }

    const Geometry::Vector<3>& Position() const { return position; }
    const Geometry::Vector<3>& Velocity() const { return velocity; }

    bool DidCollide(const BallCollider& other) const {
        return Geometry::Vector<3>::Distance(position, other.position) < Radius + other.Radius;
    }

    bool DidCollide(const BoundsCollider& other) const {
        return position.Magnitude() > other.radius - Radius;
    }

    void Collision(BallCollider& other) {
        if (&other == this || !DidCollide(other)) {
            return;
        }

        auto newVelX1 = (velocity.X() * (Mass - other.Mass) + 2 * other.Mass * other.velocity.X()) / (Mass + other.Mass);
        auto newVelZ1 = (velocity.Z() * (Mass - other.Mass) + 2 * other.Mass * other.velocity.Z()) / (Mass + other.Mass);
        auto newVelX2 = (other.velocity.X() * (other.Mass - Mass) + 2 * Mass * velocity.X()) / (Mass + other.Mass);
        auto newVelZ2 = (other.velocity.Z() * (other.Mass - Mass) + 2 * Mass * velocity.Z()) / (Mass + other.Mass);

        // Move balls to before collision
        position -= velocity;
        other.position -= other.velocity;

        // Set new velocity after collision
        velocity = { newVelX1, 0.f, newVelZ1 };
        other.velocity = { newVelX2, 0.f, newVelZ2 };
    }

    bool Collision(const BrickCollider& other) {
        if (other.Height() != 0) {
            return false;
        }

        const auto mag = position.Magnitude();
        const auto positionAngle = std::atan2(position.Z(), position.X());
        auto otherStart = other.AngleStart();
        auto otherEnd = other.AngleEnd();

        // In case end angle is smaller than -pi and ball is on positive angle, rotate pad to positive numbers
        if (otherEnd < -Geometry::pi / 2 && positionAngle > 0.f) {
            otherStart += 2 * Geometry::pi;
            otherEnd += 2 * Geometry::pi;
        } else if (otherStart > Geometry::pi / 2 && positionAngle < 0.f) {
            otherStart -= 2 * Geometry::pi;
            otherEnd -= 2 * Geometry::pi;
        }

        // Local lambda helpers
        const auto distanceToLine = [&](const float lineAngle)
        {
            const auto minusPosition = Geometry::Vector<2>() - position.To2();
            const auto line = Geometry::Vector<2>{ cos(lineAngle), sin(lineAngle) };
            return (minusPosition - Geometry::Vector<2>::Dot(minusPosition, line) * line).Magnitude();
        };

        const auto isInRing = [&]() { return mag < other.OuterRadius() && mag > other.InnerRadius(); };
        const auto isInCone = [&]() { return positionAngle < otherStart && positionAngle > otherEnd; };
        const auto isInMiddle = [&]() { return mag < other.OuterRadius() + Radius && mag > other.InnerRadius() - Radius && isInCone(); };
        const auto isOnStartWall = [&]() { return positionAngle > otherStart && distanceToLine(otherStart) < Radius && std::abs(positionAngle - otherStart) < Geometry::pi / 2; };
        const auto isOnEndWall = [&]() { return positionAngle < otherEnd && distanceToLine(otherEnd) < Radius && std::abs(positionAngle - otherEnd) < Geometry::pi / 2; };
        const auto isOnSide = [&]() { return isInRing() && (isOnStartWall() || isOnEndWall()); };
        const auto isOnCorner = [&]()
        {
            auto corners = { other.InnerStartCorner(), other.InnerEndCorner(), other.OuterStartCorner(), other.OuterEndCorner() };
            return std::any_of(corners.begin(), corners.end(), [&](const auto& corner) { return Geometry::Vector<3>::Distance(corner, position) < Radius; });
        };
        const auto didCollide = [&]()
        {
            return isInMiddle() || isOnSide() || isOnCorner();
        };

        const auto cornerCollision = [&](const Geometry::Vector<3>& corner)
        {
            const auto dist = Geometry::Vector<3>::Distance(position, corner);
            position += (position - corner) * (Radius - dist) / dist;

            return (corner - position).To2().Normalize();
        };
        const auto sideWallCollision = [&](const float lineAngle)
        {
            auto delta = distanceToLine(lineAngle) - Radius;
            unsigned i = 2;
            while (std::abs(delta) > 0.1 && i < 256) {
                delta < 0 ? position -= velocity / i : position += velocity / i;
                i *= 2;
                delta = distanceToLine(lineAngle) - Radius;
            }

            return Geometry::Vector<2>{ sin(lineAngle), -cos(lineAngle) }.Invert();
        };
        const auto outerWallCollision = [&](const float otherRadius)
        {
            auto delta = Position().Magnitude() - Radius - otherRadius;
            unsigned i = 2;
            while (std::abs(delta) > 0.1 && i < 256) {
                delta < 0 ? position -= velocity / i : position += velocity / i;
                i *= 2;
                delta = Position().Magnitude() - Radius - otherRadius;
            }
            return position.To2().Invert();
        };
        const auto innerWallCollision = [&](const float otherRadius)
        {
            auto delta = Position().Magnitude() + Radius - otherRadius;
            unsigned i = 2;
            while (std::abs(delta) > 0.1 && i < 256) {
                delta > 0 ? position -= velocity / i : position += velocity / i;
                i *= 2;
                delta = Position().Magnitude() + Radius - otherRadius;
            }

            return position.To2();
        };

        if (!didCollide()) {
            return false;
        }

        Geometry::Vector<2> normal;
        const float movementMultiplier = isInRing() ? 1.1f : 0.2f;
        if (positionAngle > otherStart) {
            if (mag > other.OuterRadius()) {
                normal = cornerCollision(other.OuterStartCorner());
            } else if (mag < other.InnerRadius()) {
                normal = cornerCollision(other.InnerStartCorner());
            } else {
                normal = sideWallCollision(otherStart);
            }
        } else if (positionAngle < otherEnd) {
            if (mag > other.OuterRadius()) {
                normal = cornerCollision(other.OuterEndCorner());
            } else if (mag < other.InnerRadius()) {
                normal = cornerCollision(other.InnerEndCorner());
            } else {
                normal = sideWallCollision(otherEnd);
            }
        } else if (mag > other.MiddleRadius()) {
            normal = outerWallCollision(other.OuterRadius());
        } else {
            normal = innerWallCollision(other.InnerRadius());
        }

        normal.Normalize();
        const auto velocity2D = velocity.To2().Invert();

        // Set new velocity
        velocity = (2.f * Geometry::Vector<2>::Dot(velocity2D, normal) * normal - velocity2D).To3();
        velocity += other.Velocity(position) * movementMultiplier;
        if (isInRing() || !isInCone()) {
            position += other.Velocity(position) * movementMultiplier;
        }
        return true;
    }

    void Collision(const BoundsCollider& other) {
        if (!DidCollide(other)) {
            return;
        }

        const auto velocity2D = velocity.To2().Invert();
        const auto normal = Geometry::Vector<2>::Inverted(position.To2()).Normalize();

        // Move ball to before collision
        position -= velocity;

        // Set new velocity
        velocity = (2.f * Geometry::Vector<2>::Dot(velocity2D, normal) * normal - velocity2D).To3();
    }
};

} // namespace Collisions
