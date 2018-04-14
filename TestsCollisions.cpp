#include "catch.hpp"

#include "collisions/Collider.hpp"
#include "Geometry"

using namespace Geometry;
using namespace Collisions;

TEST_CASE("Ball-Ball collision detection") {
    BallCollider ball{ Vector(0.f), Vector(0.f), 1.f };

    CHECK(ball.DidCollide(ball));
    CHECK(ball.DidCollide(BallCollider{ Vector(0.f), Vector(0.f), 0.f }));
    CHECK(ball.DidCollide(BallCollider{ Vector(0.f, 0.f, 1.f), Vector(0.f), 1.f }));
    CHECK(ball.DidCollide(BallCollider{ Vector(0.f, 0.f, 2.f), Vector(0.f), 1.1f }));
    CHECK_FALSE(ball.DidCollide(BallCollider{ Vector(0.f, 0.f, 2.f), Vector(0.f), 1.f }));
    CHECK_FALSE(ball.DidCollide(BallCollider{ Vector(0.f, 0.f, 2.f), Vector(0.f), 0.9f }));
}

TEST_CASE("Ball-Bounds collision detection") {
    BallCollider ball1{ Vector(0.f), Vector(0.f), 1.f };

    CHECK(ball1.DidCollide(BoundsCollider{ 0.f }));
    CHECK_FALSE(ball1.DidCollide(BoundsCollider{ 1.f }));
    CHECK_FALSE(ball1.DidCollide(BoundsCollider{ 1.5f }));

    BallCollider ball2{ Vector(0.f, 0.f, 3.f), Vector(0.f), 1.f };

    CHECK(ball2.DidCollide(BoundsCollider{ 0.f }));
    CHECK(ball2.DidCollide(BoundsCollider{ 1.f }));
    CHECK(ball2.DidCollide(BoundsCollider{ 3.5f }));
    CHECK_FALSE(ball2.DidCollide(BoundsCollider{ 4.1f }));
    CHECK_FALSE(ball2.DidCollide(BoundsCollider{ 5.f }));
}