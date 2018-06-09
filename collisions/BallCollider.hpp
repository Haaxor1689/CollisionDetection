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

    BallCollider(Geometry::Vector<3>&& position, Geometry::Vector<3>&& velocity, float radius)
        : position(std::move(position)),
          velocity(std::move(velocity)),
          Radius(radius),
          Mass(4 * Geometry::pi * radius * radius * radius * 11.34f / 3) {}

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
        if (other.Height() != 0) {
            return;
        }

        // Local lambda helpers
        const auto cornerCollision = [&](const Geometry::Vector<3>& corner)
        {
            // int i = 2;
            // while (Radius - dist > 0.1) {
            //     Radius > dist ? position -= velocity / i : position += velocity / i;
            //     i *= 2;
            //     dist = Geometry::Vector<3>::Distance(corner, position);
            // }

            return (corner - position).To2().Normalize();
        };

        const auto sideWallCollision = [](const float lineAngle)
        {
            // int i = 2;
            // while (Radius - dist > 0.1) {
            //     Radius > dist ? position -= velocity / i : position += velocity / i;
            //     i *= 2;
            //     dist = Geometry::Vector<3>::Distance(corner, position);
            // }

            return Geometry::Vector<2>{ sin(lineAngle), -cos(lineAngle) }.Invert();
            
        };

        const auto distanceToLine = [&](const float lineAngle) 
        {
            const auto minusPosition = Geometry::Vector<2>() - position.To2();
            const auto line = Geometry::Vector<2>{ cos(lineAngle), sin(lineAngle) };
            return (minusPosition - Geometry::Vector<2>::Dot(minusPosition, line) * line).Magnitude();
        };

        const auto mag = position.Magnitude();

        // Return if ball isn't inside the ring
        if (mag > other.OuterRadius() + Radius || mag < other.InnerRadius() - Radius) {
            return;
        }

        const auto positionAngle = std::atan2(position.Z(), position.X());
        auto otherStart = other.AngleStart();
        auto otherEnd = other.AngleEnd();

        if (std::abs(positionAngle - otherStart) > Geometry::pi) {
            return;
        }

        // In case end angle is smaller than -pi and ball is on positive angle, rotate pad to positive numbers
        if (otherEnd < -Geometry::pi && positionAngle > 0.f) {
            otherStart += 2 * Geometry::pi;
            otherEnd += 2 * Geometry::pi;
        }

        Geometry::Vector<2> normal;
        if (positionAngle > otherStart) {
            const auto minusPosition = Geometry::Vector<2>() - position.To2();
            if (distanceToLine(otherStart) > Radius || std::abs(positionAngle - otherStart) > 0.5) {
                // It's too far from side walls
                return;
            }

            if (mag > other.OuterRadius() && Geometry::Vector<3>::Distance(other.OuterStartCorner(), position) < Radius) {
                std::cout << "outer start collision" << std::endl;
                normal = cornerCollision(other.OuterStartCorner());
            } else if (mag < other.InnerRadius() && Geometry::Vector<3>::Distance(other.InnerStartCorner(), position) < Radius) {
                std::cout << "inner start collision" << std::endl;
                normal = cornerCollision(other.InnerStartCorner());
            } else {
                std::cout << "start wall collision" << std::endl;
                normal = sideWallCollision(otherStart);
            }
        } else if (positionAngle < otherEnd) {
            if (distanceToLine(otherEnd) > Radius || std::abs(positionAngle - otherEnd) > 0.5) {
                // It's too far from side walls
                return;
            }

            if (mag > other.OuterRadius() && Geometry::Vector<3>::Distance(other.OuterEndCorner(), position) < Radius) {
                std::cout << "outer end collision" << std::endl;
                normal = cornerCollision(other.OuterEndCorner());
            } else if (mag < other.InnerRadius() && Geometry::Vector<3>::Distance(other.InnerEndCorner(), position) < Radius) {
                std::cout << "inner end collision" << std::endl;
                normal = cornerCollision(other.InnerEndCorner());
            } else {
                std::cout << "end wall collision" << std::endl;
                normal = sideWallCollision(otherEnd);
            }
        } else if (mag > other.MiddleRadius()) {
            std::cout << "outer wall collision" << std::endl;
            normal = position.To2();
        } else {
            std::cout << "inner wall collision" << std::endl;
            normal = position.To2().Invert();
        }

        normal.Normalize();
        const auto velocity2D = velocity.To2().Invert();

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
