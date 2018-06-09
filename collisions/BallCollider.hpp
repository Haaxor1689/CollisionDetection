#pragma once

#include "Collider.hpp"
#include "BoundsCollider.hpp"
#include "BrickCollider.hpp"
#include "objects.inl"

namespace Collisions {

class BallCollider : public Collider {
    Geometry::Vector<3> position;
    Geometry::Vector<3> velocity;

public:
    const float Radius;
    const float Mass;

    BallCollider(Geometry::Vector<3>&& position, Geometry::Vector<3>&& velocity, float radius)
        : position(std::move(position)), velocity(std::move(velocity)), Radius(radius), Mass(4 * Geometry::pi * radius * radius * radius * 11.34f / 3) {}

    // Visitors
    void Visit(ColliderVisitor& visitor) override { visitor(*this); }
    void Visit(ConstColliderVisitor& visitor) const override { visitor(*this); }

    void Step() { position += velocity; }

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

    void Collision(const BrickCollider& other) {
        const auto mag = position.Magnitude();

        // Return if ball isn't inside the ring
        if (mag > other.OuterRadius() + Radius || mag < other.InnerRadius() - Radius) {
            return;
        }

        const auto positionAngle = std::atan2(position.Z(), position.X());
        auto otherStart = other.AngleStart();
        auto otherEnd = other.AngleEnd();

        // In case end angle is smaller than -pi and ball is on positive angle, rotate pad to positive numbers
        if (otherEnd < -Geometry::pi && positionAngle > 0.f) {
            otherStart += 2 * Geometry::pi;
            otherEnd += 2 * Geometry::pi;
        }

        Geometry::Vector<2> normal;
        if (positionAngle > otherStart) {
            const auto minusPosition = Geometry::Vector<2>() - position.To2();
            const auto line = Geometry::Vector<2>{ cos(otherStart), sin(otherStart) };
            auto distance = minusPosition - Geometry::Vector<2>::Dot(minusPosition, line) * line;
            if (distance.Magnitude() > Radius) {
                return;
            }

            normal = Geometry::Vector<2>{ sin(otherStart), -cos(otherStart) };
        } else if (positionAngle < otherEnd) {
            const auto minusPosition = Geometry::Vector<2>() - position.To2();
            const auto line = Geometry::Vector<2>{ cos(otherEnd), sin(otherEnd) };
            auto distance = minusPosition - Geometry::Vector<2>::Dot(minusPosition, line) * line;
            if (distance.Magnitude() > Radius) {
                return;
            }

            normal = Geometry::Vector<2>{ -sin(otherEnd), cos(otherEnd) };
        } else {
            // Ball is inside brick cone
            normal = position.To2().Normalize();

            if (mag < other.MiddleRadius()) {
                // Collision with outer wall
                normal.Invert();
            }
        }
        const auto velocity2D = velocity.To2().Invert();

        // Move ball to before collision
        position -= velocity;

        // Set new velocity
        velocity = (2.f * Geometry::Vector<2>::Dot(velocity2D, normal) * normal - velocity2D).To3();
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