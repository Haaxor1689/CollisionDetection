#include "catch.hpp"

#include "Collisions"
#include "Geometry"

using namespace Geometry;
using namespace Collisions;

TEST_CASE("Ball-Ball collision detection") {
    BallCollider ball{ Vector<3>(), Vector<3>(), 1.f };

    CHECK(ball.DidCollide(ball));
    CHECK(ball.DidCollide(BallCollider{ Vector<3>(), Vector<3>(), 0.f }));
    CHECK(ball.DidCollide(BallCollider{ Vector<3>{ 0.f, 0.f, 1.f }, Vector<3>(), 1.f }));
    CHECK(ball.DidCollide(BallCollider{ Vector<3>{ 0.f, 0.f, 2.f }, Vector<3>(), 1.1f }));
    CHECK_FALSE(ball.DidCollide(BallCollider{ Vector<3>{ 0.f, 0.f, 2.f }, Vector<3>(), 1.f }));
    CHECK_FALSE(ball.DidCollide(BallCollider{ Vector<3>{ 0.f, 0.f, 2.f }, Vector<3>(), 0.9f }));
}

TEST_CASE("Ball-Bounds collision detection") {
    BallCollider ball1{ Vector<3>(0.f), Vector<3>(), 1.f };

    CHECK(ball1.DidCollide(BoundsCollider{ 0.f }));
    CHECK_FALSE(ball1.DidCollide(BoundsCollider{ 1.f }));
    CHECK_FALSE(ball1.DidCollide(BoundsCollider{ 1.5f }));

    BallCollider ball2{ Vector<3>{ 0.f, 0.f, 3.f }, Vector<3>(), 1.f };

    CHECK(ball2.DidCollide(BoundsCollider{ 0.f }));
    CHECK(ball2.DidCollide(BoundsCollider{ 1.f }));
    CHECK(ball2.DidCollide(BoundsCollider{ 3.5f }));
    CHECK_FALSE(ball2.DidCollide(BoundsCollider{ 4.1f }));
    CHECK_FALSE(ball2.DidCollide(BoundsCollider{ 5.f }));
}

TEST_CASE("Ball-Brick collision detection") {
    BallCollider ball1{ Vector<3>{ 5.f, 0.f, 0.f }, Vector<3>(), 1.f };

    CHECK_FALSE(ball1.DidCollide(BrickCollider{ 0.5f, segments / 4 }));
    CHECK_FALSE(ball1.DidCollide(BrickCollider{ 6.5f, segments / 4 }));
    CHECK(ball1.DidCollide(BrickCollider{ 2.9f, segments / 4 }));
    CHECK(ball1.DidCollide(BrickCollider{ 3.3f, segments / 4 }));

    BallCollider ball2{ Vector<3>{ 3.f, 0.f, 1.1f }, Vector<3>(), 1.f };

    CHECK_FALSE(ball2.DidCollide(BrickCollider{ 0.5f, segments / 4 }));
    CHECK_FALSE(ball2.DidCollide(BrickCollider{ 4.5f, segments / 4 }));
    CHECK_FALSE(ball2.DidCollide(BrickCollider{ 1.9f, segments / 4 }));
    CHECK_FALSE(ball2.DidCollide(BrickCollider{ 3.3f, segments / 4 }));

    BrickCollider brick1{ 2.9f, segments / 4 };

    CHECK(BallCollider{ Vector<3>{ 2.9f, 0.f, -1.1f }, Vector<3>(), 1.f }.DidCollide(brick1));
    CHECK_FALSE(BallCollider{ Vector<3>{ -1.1f, 0.f, -2.9f }, Vector<3>(), 1.f }.DidCollide(brick1));
    CHECK_FALSE(BallCollider{ Vector<3>{ -1.1f, 0.f, 2.9f }, Vector<3>(), 1.f }.DidCollide(brick1));
}