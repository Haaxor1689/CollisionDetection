#include "catch.hpp"

#include <random>
#include <type_traits>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "Geometry"

using namespace Geometry;

#define GLMOperators(size)                                                                        \
    bool operator==(const Matrix<size>& lhs, const glm::mat##size& rhs) {                         \
        for (size_t j = 0; j < size; ++j)                                                         \
            for (size_t i = 0; i < size; ++i)                                                     \
                if (lhs[i][j] != rhs[i][j])                                                       \
                    return false;                                                                 \
        return true;                                                                              \
    }                                                                                             \
    bool operator!=(const Matrix<size>& lhs, const glm::mat##size& rhs) { return !(lhs == rhs); } \
    std::ostream& operator<<(std::ostream& os, const glm::mat##size& mat) {                       \
        auto ptr = glm::value_ptr(mat);                                                           \
        os << "{ ";                                                                               \
        for (size_t i = 0; i < size * size; ++i) {                                                \
            os << *ptr << ", ";                                                                   \
            ++ptr;                                                                                \
        }                                                                                         \
        return os << "}";                                                                         \
    }

namespace {
std::random_device r;
std::default_random_engine e(r());
std::uniform_real_distribution<float> rrd(-100, 100);

float RandomFloat() {
    return rrd(e);
}

Vector ToVector(const glm::vec3& vec) {
    return { vec.x, vec.y, vec.z };
}

void CHECK_APPROX(const Vector& lhs, const Vector& rhs) {
    CHECK(lhs.x() == Approx(rhs.x()).epsilon(0.001f));
    CHECK(lhs.y() == Approx(rhs.y()).epsilon(0.001f));
    CHECK(lhs.z() == Approx(rhs.z()).epsilon(0.001f));
}

GLMOperators(2);
GLMOperators(3);
GLMOperators(4);
} // namespace

TEST_CASE("Vector") {
    SECTION("Construction") {
        auto cons = std::is_default_constructible_v<Vector>;
        REQUIRE(cons);

        cons = std::is_constructible_v<Vector, float>;
        REQUIRE(cons);

        cons = std::is_constructible_v<Vector, float, float, float>;
        REQUIRE(cons);

        Vector a;
        CHECK(a.x() == 0.f);
        CHECK(a.y() == 0.f);
        CHECK(a.z() == 0.f);

        Vector b(1.f, 2.f, 3.f);
        CHECK(b.x() == 1.f);
        CHECK(b.y() == 2.f);
        CHECK(b.z() == 3.f);

        Vector c(2.f);
        CHECK(c.x() == 2.f);
        CHECK(c.y() == 2.f);
        CHECK(c.z() == 2.f);
    }

    SECTION("Copy ctor") {
        Vector a(1.f, 2.f, 3.f);
        Vector b(a);
        CHECK(b.x() == 1.f);
        CHECK(b.y() == 2.f);
        CHECK(b.z() == 3.f);
        Vector c(Vector(4.f, 5.f, 6.f));
        CHECK(c.x() == 4.f);
        CHECK(c.y() == 5.f);
        CHECK(c.z() == 6.f);
    }

    SECTION("Copy assignment") {
        Vector a(1.f, 2.f, 3.f);
        Vector b(4.f, 5.f, 6.f);
        b = a;
        CHECK(b.x() == 1.f);
        CHECK(b.y() == 2.f);
        CHECK(b.z() == 3.f);
        b = Vector(7.f, 8.f, 9.f);
        CHECK(b.x() == 7.f);
        CHECK(b.y() == 8.f);
        CHECK(b.z() == 9.f);
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
            CHECK(Vector(x, y, z).Magnitude() == mag(x, y, z));
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
            CHECK(Vector(x, y, z).Normalize() == norm(x, y, z));
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
            CHECK(Vector(x, y, z).Invert() == inv(x, y, z));
        }
    }

    SECTION("Dot product") {
        CHECK(Vector::Dot(Vector(), Vector()) == 0);
        CHECK(Vector::Dot(Vector(1.f), Vector(1.f)) == 3.f);

        auto dot = [](const Vector& lhs, const Vector& rhs) {
            return lhs.x() * rhs.x() + lhs.y() * rhs.y() + lhs.z() * rhs.z();
        };

        for (unsigned i = 0; i < 100; ++i) {
            float x = RandomFloat();
            float y = RandomFloat();
            float z = RandomFloat();
            float p = RandomFloat();
            float q = RandomFloat();
            float r = RandomFloat();
            CHECK(Vector::Dot({ x, y, z }, { p, q, r }) == dot({ x, y, z }, { p, q, r }));
        }
    }

    SECTION("Cross product") {
        CHECK(Vector::Cross(Vector(), Vector()) == Vector());
        CHECK(Vector::Cross({ 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }) == Vector(0.f, 0.f, 1.f));

        auto cross = [](const Vector& lhs, const Vector& rhs) {
            return Vector(lhs.y() * rhs.z() - rhs.y() * lhs.z(), lhs.z() * rhs.x() - rhs.z() * lhs.x(), lhs.x() * rhs.y() - rhs.x() * lhs.y());
        };

        for (unsigned i = 0; i < 100; ++i) {
            float x = RandomFloat();
            float y = RandomFloat();
            float z = RandomFloat();
            float p = RandomFloat();
            float q = RandomFloat();
            float r = RandomFloat();
            CHECK(Vector::Cross({ x, y, z }, { p, q, r }) == cross({ x, y, z }, { p, q, r }));
        }
    }

    SECTION("Matrix multiplication") {
        Vector vec = { 1.f, 2.f, 3.f };
        Matrix<3> mat = {
            1.f, 2.f, 3.f,
            4.f, 5.f, 6.f,
            7.f, 8.f, 9.f
        };

        Vector expected = { 14.f, 32.f, 50.f };
        CHECK(vec * mat == expected);
    }

    SECTION("Translation, rotation and scaling") {
        for (unsigned i = 0; i < 100; ++i) {
            float x = RandomFloat();
            float y = RandomFloat();
            float z = RandomFloat();
            float p = RandomFloat();
            float q = RandomFloat();
            float r = RandomFloat();
            float angle = RandomFloat();
            CHECK(Vector{ x, y, z }.Translate({ p, q, r }) == Vector{ x, y, z } + Vector{ p, q, r });
            CHECK_APPROX(Vector{ x, y, z }.Rotate(angle, { p, q, r }), ToVector(glm::rotate(glm::vec3(x, y, z), angle, glm::vec3(p, q, r))));
            CHECK(Vector{ x, y, z }.Scale({ p, q, r }) == Vector{ x, y, z } * Vector{ p, q, r });
        }
    }
}

TEST_CASE("Matrix") {
    SECTION("Construction") {
        auto cons = std::is_default_constructible_v<Matrix<3>>;
        REQUIRE(cons);

        cons = std::is_constructible_v<Matrix<3>, float>;
        REQUIRE(cons);

        cons = std::is_constructible_v<Matrix<3>, std::initializer_list<float>>;
        REQUIRE(cons);

        CHECK(Matrix<3>() == std::initializer_list<float>{ 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f });
        CHECK(Matrix<3>(1.f) == std::initializer_list<float>{ 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f });

        Matrix<3> a = {
            1.f, 2.f, 3.f,
            4.f, 5.f, 6.f,
            7.f, 8.f, 9.f
        };
        CHECK(a == std::initializer_list<float>{ 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f });

        Matrix<3> b = a;
        CHECK(b == std::initializer_list<float>{ 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f });

        Matrix<3> c(b);
        CHECK(c == std::initializer_list<float>{ 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f });

        CHECK_THROWS_WITH(Matrix<3>({ 1.f, 1.f }), "Can't initialize with list of size 2. Size must be 9.");
        CHECK_THROWS_WITH(Matrix<3>({ 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f }), "Can't initialize with list of size 10. Size must be 9.");
    }

    SECTION("Matrix arithmetic operators") {
        Matrix<3> a = {
            1.f, 2.f, 3.f,
            4.f, 5.f, 6.f,
            7.f, 8.f, 9.f
        };

        Matrix<3> b = a + a;
        CHECK(b == Matrix<3>({ 2.f, 4.f, 6.f, 8.f, 10.f, 12.f, 14.f, 16.f, 18.f }));
        CHECK(a == Matrix<3>({ 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f }));

        Matrix<3> c = b - a;
        CHECK(c == a);
        CHECK(b == Matrix<3>({ 2.f, 4.f, 6.f, 8.f, 10.f, 12.f, 14.f, 16.f, 18.f }));
    }

    SECTION("Index operator") {
        Matrix<3> a = {
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
        Matrix<3> a = {
            1.f, 2.f, 3.f,
            4.f, 5.f, 6.f,
            7.f, 8.f, 9.f
        };
        Matrix<3> b = {
            9.f, 8.f, 7.f,
            6.f, 5.f, 4.f,
            3.f, 2.f, 1.f
        };

        CHECK(a * b == Matrix<3>({ 30.f, 24.f, 18.f, 84.f, 69.f, 54.f, 138.f, 114.f, 90.f }));
        CHECK(b * a == Matrix<3>({ 90.f, 114.f, 138.f, 54.f, 69.f, 84.f, 18.f, 24.f, 30.f }));
    }

    SECTION("Identity matrix") {
        CHECK(Matrix<3>::Identity() == glm::mat3(1.f));
        CHECK(Matrix<4>::Identity() == glm::mat4(1.f));
    }

    SECTION("Transposed matrix") {
        Matrix<3> a = {
            1.f, 2.f, 3.f,
            4.f, 5.f, 6.f,
            7.f, 8.f, 9.f
        };
        Matrix<3> b = {
            1.f, 4.f, 7.f,
            2.f, 5.f, 8.f,
            3.f, 6.f, 9.f
        };

        CHECK(a.Transpose() == b);
    }

    SECTION("Minor matrix") {
        Matrix<4> a = {
            1.f, 2.f, 3.f, 4.f,
            5.f, 6.f, 7.f, 8.f,
            9.f, 10.f, 11.f, 12.f,
            13.f, 14.f, 15.f, 16.f
        };
        Matrix<3> b = {
            5.f, 6.f, 8.f,
            9.f, 10.f, 12.f,
            13.f, 14.f, 16.f
        };
        Matrix<2> c = {
            5.f, 6.f,
            9.f, 10.f
        };

        CHECK(a.Minor(0, 2) == b);
        CHECK(b.Minor(2, 2) == c);
    }

    SECTION("Matrix determinant") {
        Matrix<2> a = {
            1.f, 2.f,
            3.f, 4.f
        };
        CHECK(a.Determinant() == -2.f);

        Matrix<3> b = {
            1.f, 2.f, 3.f,
            3.f, 2.f, 1.f,
            1.f, 3.f, 2.f
        };
        CHECK(b.Determinant() == 12.f);

        Matrix<4> c = {
            1.f, 2.f, 3.f, 1.f,
            3.f, 4.f, 5.f, 2.f,
            2.f, 5.f, 4.f, 3.f,
            1.f, 3.f, 2.f, 1.f
        };
        CHECK(c.Determinant() == -7.f);
    }

    SECTION("Matrix of minors") {
        Matrix<3> a = {
            3.f, 0.f, 2.f,
            2.f, 0.f, -2.f,
            0.f, 1.f, 1.f
        };
        Matrix<3> b = {
            2.f, 2.f, 2.f,
            -2.f, 3.f, 3.f,
            0.f, -10.f, 0.f
        };

        CHECK(Matrix<3>::Minors(a) == b);
    }

    SECTION("Matrix of cofactors") {
        Matrix<3> a = {
            1.f, 2.f, 3.f,
            4.f, 5.f, 6.f,
            7.f, 8.f, 9.f
        };
        Matrix<3> b = {
            1.f, -2.f, 3.f,
            -4.f, 5.f, -6.f,
            7.f, -8.f, 9.f
        };
        CHECK(a.Cofactor() == b);

        Matrix<4> c = {
            1.f, 2.f, 3.f, 1.f,
            3.f, 4.f, 5.f, 2.f,
            2.f, 5.f, 4.f, 3.f,
            1.f, 3.f, 2.f, 1.f
        };
        Matrix<4> d = {
            1.f, -2.f, 3.f, -1.f,
            -3.f, 4.f, -5.f, 2.f,
            2.f, -5.f, 4.f, -3.f,
            -1.f, 3.f, -2.f, 1.f
        };
        CHECK(c.Cofactor() == d);
    }

    SECTION("Matrix inverse") {
        Matrix<3> a = {
            3.f, 0.f, 2.f,
            2.f, 0.f, -2.f,
            0.f, 1.f, 1.f
        };
        Matrix<3> b = {
            .2f, .2f, 0.f,
            -.2f, .3f, 1.f,
            .2f, -.3f, 0.f
        };
        CHECK(a.Invert() == b);

        Matrix<4> c = {
            1.f, 2.f, 3.f, 1.f,
            3.f, 4.f, 5.f, 2.f,
            2.f, 5.f, 4.f, 3.f,
            1.f, 3.f, 2.f, 1.f
        };
        Matrix<4> d = {
            -1.14285714285714f, 0.857142857142857f, -0.142857142857143f, -0.142857142857143f,
            -0.142857142857143f, -0.142857142857143f, -0.142857142857143f, 0.857142857142857f,
            0.857142857142857f, -0.142857142857143f, -0.142857142857143f, -0.142857142857143f,
            -0.142857142857143f, -0.142857142857143f, 0.857142857142857f, -1.14285714285714f
        };
        CHECK(c.Invert() == d);
    }

    SECTION("Perspective") {
    }
}

TEST_CASE("Ray") {
}

TEST_CASE("Utility") {
    SECTION("Radians") {
        CHECK(Radians(10.f) == Approx(glm::radians(10.f)).epsilon(0.0001f));
        float angle = 150.f;
        CHECK(Radians(angle) == Approx(glm::radians(angle)).epsilon(0.0001f));
    }
}