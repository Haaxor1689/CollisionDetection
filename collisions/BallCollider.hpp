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
        : position(std::move(position)), velocity(std::move(velocity)), radius(radius), mass(4 * Geometry::Pi() * radius * radius * radius * 11.34f / 3) {}

    // Visitors
    void visit(ColliderVisitor& visitor) { visitor(*this); }
    void visit(ConstColliderVisitor& visitor) const { visitor(*this); }

    void Step() {
        position += velocity;
    }

    const Geometry::Vector<3>& Position() const { return position; }
    const Geometry::Vector<3>& Velocity() const { return velocity; }

    bool DidCollide(const BallCollider& other) const {
        return Geometry::Vector<3>::Distance(position, other.position) < radius + other.radius;
    }

    bool DidCollide(const BrickCollider& other) const {
        float mag = position.Magnitude();
        if (mag < other.InnerRadius() - radius ||
            mag > other.OuterRadius() + radius) {
            return false;
        }

        auto dot1 = Geometry::Vector<3>::Dot(position, other.StartPoint());
        auto projected1 = dot1 / Geometry::Vector<3>::Dot(other.StartPoint(), other.StartPoint()) * other.StartPoint();
        auto distance1 = Geometry::Vector<3>::Distance(position, projected1);

        auto side1 = position.x() * other.StartPoint().z() - position.z() * other.StartPoint().x();

        auto dot2 = Geometry::Vector<3>::Dot(position, other.EndPoint());
        auto projected2 = dot2 / Geometry::Vector<3>::Dot(other.EndPoint(), other.EndPoint()) * other.EndPoint();
        auto distance2 = Geometry::Vector<3>::Distance(position, projected2);

        auto side2 = position.x() * other.EndPoint().z() - position.z() * other.EndPoint().x();

        return distance1 < radius || distance2 < radius || (side1 > 0.f && side2 < 0.f);
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
        if (!DidCollide(other)) {
            return;
        }
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