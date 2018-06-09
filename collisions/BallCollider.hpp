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
    const float radius;
    const float mass;

    BallCollider(Geometry::Vector<3>&& position, Geometry::Vector<3>&& velocity, float radius)
        : position(std::move(position)), velocity(std::move(velocity)), radius(radius), mass(4 * Geometry::Pi * radius * radius * radius * 11.34f / 3) {}

    // Visitors
    void visit(ColliderVisitor& visitor) { visitor(*this); }
    void visit(ConstColliderVisitor& visitor) const { visitor(*this); }

    void Step() { position += velocity; }

    const Geometry::Vector<3>& Position() const { return position; }
    const Geometry::Vector<3>& Velocity() const { return velocity; }

    bool DidCollide(const BallCollider& other) const {
        return Geometry::Vector<3>::Distance(position, other.position) < radius + other.radius;
    }

    bool DidCollide(const BoundsCollider& other) const {
        return position.Magnitude() > other.radius - radius;
    }

    void Collision(BallCollider& other) {
        if (&other == this || !DidCollide(other)) {
            return;
        }

        float newVelX1 = (velocity.x() * (mass - other.mass) + (2 * other.mass * other.velocity.x())) / (mass + other.mass);
        float newVelZ1 = (velocity.z() * (mass - other.mass) + (2 * other.mass * other.velocity.z())) / (mass + other.mass);
        float newVelX2 = (other.velocity.x() * (other.mass - mass) + (2 * mass * velocity.x())) / (mass + other.mass);
        float newVelZ2 = (other.velocity.z() * (other.mass - mass) + (2 * mass * velocity.z())) / (mass + other.mass);

        // Move balls to before collision
        position -= velocity;
        other.position -= other.velocity;

        // Set new velocity after collision
        velocity = { newVelX1, 0.f, newVelZ1 };
        other.velocity = { newVelX2, 0.f, newVelZ2 };
    }

    void Collision(const BrickCollider& other) {
        float mag = position.Magnitude();

        // Return if ball isn't inside the ring
        if (mag > other.OuterRadius() + radius || mag < other.InnerRadius() - radius) {
            return;
        }

        float positionAngle = std::atan2(position.z(), position.x());
        Geometry::Vector<2> normal;
        if (positionAngle > other.AngleStart()) {
            auto minusPosition = Geometry::Vector<2>() - position.To2();
            auto line = Geometry::Vector<2>{ cos(other.AngleStart()), sin(other.AngleStart()) };
            auto distance = minusPosition - (Geometry::Vector<2>::Dot(minusPosition, line) * line);
            if (distance.Magnitude() > radius) {
                return;
            }

            normal = Geometry::Vector<2>{ sin(other.AngleStart()), -cos(other.AngleStart()) };
        } else if (positionAngle < other.AngleEnd()) {
            auto minusPosition = Geometry::Vector<2>() - position.To2();
            auto line = Geometry::Vector<2>{ cos(other.AngleEnd()), sin(other.AngleEnd()) };
            auto distance = minusPosition - (Geometry::Vector<2>::Dot(minusPosition, line) * line);
            if (distance.Magnitude() > radius) {
                return;
            }

            normal = Geometry::Vector<2>{ -sin(other.AngleEnd()), cos(other.AngleEnd()) };
        } else {
            // Ball is inside brick cone
            normal = position.To2().Normalize();

            if (mag < other.MiddleRadius()) {
                // Collision with outer wall
                normal.Invert();
            }
        }
        auto velocity2D = velocity.To2().Invert();

        // Move ball to before collision
        position -= velocity;

        // Set new velocity
        velocity = (2.f * Geometry::Vector<2>::Dot(velocity2D, normal) * normal - velocity2D).To3();
    }

    void Collision(const BoundsCollider& other) {
        if (!DidCollide(other)) {
            return;
        }

        auto velocity2D = velocity.To2().Invert();
        auto normal = Geometry::Vector<2>::Inverted(position.To2()).Normalize();

        // Move ball to before collision
        position -= velocity;

        // Set new velocity
        velocity = (2.f * Geometry::Vector<2>::Dot(velocity2D, normal) * normal - velocity2D).To3();
    }
};

} // namespace Collisions