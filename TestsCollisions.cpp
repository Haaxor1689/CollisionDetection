#include "catch.hpp"

#include "collisions/Collider.hpp"
#include "Geometry"

using namespace Geometry;
using namespace Collisions;

TEST_CASE("Ball-Ball collision detection") {
    BallCollider ball1{ Vector(0.f), Vector(0.f), 1.f };

    CHECK(ball1.DidCollide(ball1));
    CHECK(ball1.DidCollide({ Vector(0.f), Vector(0.f), 0.f }));
    CHECK(ball1.DidCollide({ Vector(0.f, 0.f, 1.f), Vector(0.f), 1.f }));
    CHECK(ball1.DidCollide({ Vector(0.f, 0.f, 2.f), Vector(0.f), 1.1f }));
    CHECK_FALSE(ball1.DidCollide({ Vector(0.f, 0.f, 2.f), Vector(0.f), 1.f }));
    CHECK_FALSE(ball1.DidCollide({ Vector(0.f, 0.f, 2.f), Vector(0.f), 0.9f }));
}