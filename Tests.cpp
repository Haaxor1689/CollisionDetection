#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "MyEngine.hpp"
#include <random>

std::random_device r;
std::default_random_engine e(r());
std::uniform_real_distribution< float > rrd(-100, 100);

float RandomFloat() {
    return rrd(e);
}

using namespace MyEngine;

TEST_CASE("Vector") {
    SECTION("Construction") {
        Vector a;
        CHECK(a.x == 0.f);
        CHECK(a.y == 0.f);
        CHECK(a.z == 0.f);
        
        Vector b(1.f, 2.f ,3.f);
        CHECK(b.x == 1.f);
        CHECK(b.y == 2.f);
        CHECK(b.z == 3.f);
    }

    SECTION("Copy ctor") {
        Vector a(1.f, 2.f ,3.f);
        Vector b(a);
        CHECK(b.x == 1.f);
        CHECK(b.y == 2.f);
        CHECK(b.z == 3.f);
        Vector c(Vector(4.f, 5.f, 6.f));
        CHECK(c.x == 4.f);
        CHECK(c.y == 5.f);
        CHECK(c.z == 6.f);
    }

    SECTION("Copy assignment") {
        Vector a(1.f, 2.f, 3.f);
        Vector b(4.f, 5.f, 6.f);
        b = a;
        CHECK(b.x == 1.f);
        CHECK(b.y == 2.f);
        CHECK(b.z == 3.f);
        b = Vector(7.f, 8.f, 9.f);
        CHECK(b.x == 7.f);
        CHECK(b.y == 8.f);
        CHECK(b.z == 9.f);
    }

    SECTION("Addition and subtraction") {
        Vector a(1.f, 2.f, 3.f);
        Vector b = a + a;
        CHECK(b == Vector(2.f, 4.f, 6.f));
        Vector c = b - a;
        CHECK(c == a);
    }

    SECTION("Magnitude") {
        CHECK(Vector().Magnitude() == 0.0f);
        Vector a(1.f, 2.f, 3.f);
        CHECK(a.Magnitude() == std::sqrt(14.f));
        Vector b(3.f, 2.f, 1.f);
        CHECK(b.Magnitude() == a.Magnitude());

        auto mag = [](float x, float y, float z) {
            return std::sqrt(x * x + y * y + z * z);
        };

        for (unsigned i = 0; i < 100; ++i) {
            float x = RandomFloat();
            float y = RandomFloat();
            float z = RandomFloat();
            CHECK(Vector(x, y ,z).Magnitude() == mag(x, y, z));
        }
    }

    SECTION("Normalization") {
        CHECK(Vector().Normalize() == Vector());

        Vector a(2.f, 1.f, 6.f);
        Vector b = Vector::Normalized(a);
        CHECK(a.Normalize() == b);

        auto norm = [](float x, float y, float z) {
            float mag = std::sqrt(x * x + y * y + z * z);
            if (mag == 0)
                return Vector();
            return Vector(x / mag, y / mag, z / mag);
        };

        for (unsigned i = 0; i < 100; ++i) {
            float x = RandomFloat();
            float y = RandomFloat();
            float z = RandomFloat();
            CHECK(Vector(x, y ,z).Normalize() == norm(x, y, z));
        }
    }
}

TEST_CASE("Matrix") {

}

TEST_CASE("Ray") {

}