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
        auto cons = std::is_default_constructible_v<Vector>;
        REQUIRE(cons);

        cons = std::is_constructible_v<Vector, float>;
        REQUIRE(cons);

        cons = std::is_constructible_v<Vector, float, float, float>;
        REQUIRE(cons);

        Vector a;
        CHECK(a.x == 0.f);
        CHECK(a.y == 0.f);
        CHECK(a.z == 0.f);
        
        Vector b(1.f, 2.f ,3.f);
        CHECK(b.x == 1.f);
        CHECK(b.y == 2.f);
        CHECK(b.z == 3.f);

        Vector c(2.f);
        CHECK(c.x == 2.f);
        CHECK(c.y == 2.f);
        CHECK(c.z == 2.f);
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

    SECTION("Vector arithmetic operators") {
        Vector a(1.f, 2.f, 3.f);
        Vector b = a + a;
        CHECK(b == Vector(2.f, 4.f, 6.f));
        CHECK(a == Vector(1.f, 2.f, 3.f));
        Vector c = b - a;
        CHECK(c == a);
        CHECK(b == Vector(2.f, 4.f, 6.f));
    }

    SECTION("Magnitude") {
        CHECK(Vector().Magnitude() == 0.f);
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
            if (mag == 0.f)
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

    SECTION("Invert") {
        CHECK(Vector().Invert() == Vector());

        Vector a(1.f, 2.f, 3.f);
        Vector b(-1.f, -2.f, -3.f);
        CHECK(a.Invert() == b);


        auto inv = [](float x, float y, float z) {
            return Vector(-x, -y, -z);
        };

        for (unsigned i = 0; i < 100; ++i) {
            float x = RandomFloat();
            float y = RandomFloat();
            float z = RandomFloat();
            CHECK(Vector(x, y ,z).Invert() == inv(x, y, z));
        }
    }

    SECTION("Dot product") {
        CHECK(Vector::Dot(Vector(), Vector()) == 0);
        CHECK(Vector::Dot(Vector(1.f), Vector(1.f)) == 3.f);

        auto dot = [](const Vector& lhs, const Vector& rhs) {
            return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z; 
        };

        for (unsigned i = 0; i < 100; ++i) {
            float x = RandomFloat();
            float y = RandomFloat();
            float z = RandomFloat();
            float p = RandomFloat();
            float q = RandomFloat();
            float r = RandomFloat();
            CHECK(Vector::Dot({ x, y, z }, { p, q, r })== dot({ x, y, z }, { p, q, r }));
        }
    }

    SECTION("Cross product") {
        CHECK(Vector::Cross(Vector(), Vector()) == Vector());
        CHECK(Vector::Cross({ 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }) == Vector(0.f, 0.f, 1.f));

        auto cross = [](const Vector& lhs, const Vector& rhs) {
            return Vector(lhs.y * rhs.z - rhs.y * lhs.z, lhs.z * rhs.x - rhs.z * lhs.x, lhs.x * rhs.y - rhs.x * lhs.y );
        };

        for (unsigned i = 0; i < 100; ++i) {
            float x = RandomFloat();
            float y = RandomFloat();
            float z = RandomFloat();
            float p = RandomFloat();
            float q = RandomFloat();
            float r = RandomFloat();
            CHECK(Vector::Cross({ x, y, z }, { p, q, r })== cross({ x, y, z }, { p, q, r }));
        }
    }
}

TEST_CASE("Matrix") {
    SECTION("Construction") {
        auto cons = std::is_default_constructible_v<Matrix>;
        REQUIRE(cons);

        cons = std::is_constructible_v<Matrix, float>;
        REQUIRE(cons);

        cons = std::is_constructible_v<Matrix, std::initializer_list<float>>;
        REQUIRE(cons);
        
        CHECK(Matrix() == std::initializer_list<float>{ 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f });
        CHECK(Matrix(1.f) == std::initializer_list<float>{ 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f });

        Matrix a = {
            1.f, 2.f, 3.f,
            4.f, 5.f, 6.f,
            7.f, 8.f, 9.f
        };
        CHECK(a == std::initializer_list<float>{ 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f });

        Matrix b = a;
        CHECK(b == std::initializer_list<float>{ 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f });

        Matrix c(b);
        CHECK(c == std::initializer_list<float>{ 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f });

        CHECK_THROWS_WITH(Matrix({ 1.f, 1.f }), "Can't initialize with list of size 2. Size must be 9.");
        CHECK_THROWS_WITH(Matrix({ 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f }), "Can't initialize with list of size 10. Size must be 9.");
    }

    SECTION("Matrix arithmetic operators") {
        Matrix a = {
            1.f, 2.f, 3.f,
            4.f, 5.f, 6.f,
            7.f, 8.f, 9.f
        };
        Matrix b = a + a;
        CHECK(b == Matrix({ 2.f, 4.f, 6.f, 8.f, 10.f, 12.f, 14.f, 16.f, 18.f }));
        CHECK(a == Matrix({ 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f }));

        Matrix c = b - a;
        CHECK(c == a);
        CHECK(b == Matrix({ 2.f, 4.f, 6.f, 8.f, 10.f, 12.f, 14.f, 16.f, 18.f }));
    }

    SECTION("Index operator") {
        Matrix a = {
            1.f, 2.f, 3.f,
            4.f, 5.f, 6.f,
            7.f, 8.f, 9.f
        };

        CHECK(a[0][0] == 1.f);
        CHECK(a[1][0] == 2.f);
        CHECK(a[2][0] == 3.f);
        CHECK(a[0][1] == 4.f);
        CHECK(a[1][1] == 5.f);
        CHECK(a[2][1] == 6.f);
        CHECK(a[0][2] == 7.f);
        CHECK(a[1][2] == 8.f);
        CHECK(a[2][2] == 9.f);

        a[2][0] = 0.f;
        CHECK(a[2][0] == 0.f);
        CHECK(a == std::initializer_list<float>{ 1.f, 2.f, 0.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f });
    }

    SECTION("Matrix multiplication") {
        Matrix a = {
            1.f, 2.f, 3.f,
            4.f, 5.f, 6.f,
            7.f, 8.f, 9.f
        };
        Matrix b = {
            3.f, 2.f, 1.f,
            6.f, 5.f, 4.f,
            9.f, 8.f, 7.f
        };

        CHECK(a * b == Matrix({ 42.f, 36.f, 30.f, 96.f, 81.f, 66.f, 150.f, 126.f, 102.f }));
        CHECK(b * a == Matrix({ 18.f, 24.f, 30.f, 54.f, 69.f, 84.f, 90.f, 114.f, 113.f }));
    }
}

TEST_CASE("Ray") {

}