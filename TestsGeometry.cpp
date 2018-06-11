#include "catch.hpp"

#include <random>
#include <type_traits>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "Geometry"
#include <iostream>

using namespace Geometry;
#define GLMOperators(size)                                                                        \
    bool operator==(const Matrix<size>& lhs, const glm::mat##size& rhs) {                         \
        for (size_t j = 0; j < size; ++j)                                                         \
            for (size_t i = 0; i < size; ++i)                                                     \
                if (std::abs(lhs[i][j] - rhs[j][i]) > 0.001f)                                                       \
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

Vector<3> ToVector(const glm::vec3& vec) {
    return { vec.x, vec.y, vec.z };
}

template <size_t Size>
void REQUIRE_APPROX(const Vector<Size>& lhs, const Vector<Size>& rhs) {
    REQUIRE(lhs.X() == Approx(rhs.X()).epsilon(0.001f));
    REQUIRE(lhs.Y() == Approx(rhs.Y()).epsilon(0.001f));
    REQUIRE(lhs.Z() == Approx(rhs.Z()).epsilon(0.001f));
}

GLMOperators(2);
GLMOperators(3);
GLMOperators(4);
} // namespace

TEST_CASE("Vector") {
    SECTION("Construction") {
        auto cons = std::is_default_constructible_v<Vector<3>>;
        REQUIRE(cons);

        cons = std::is_constructible_v<Vector<3>, float>;
        REQUIRE(cons);

        Vector<3> a;
        CHECK(a.X() == 0.f);
        CHECK(a.Y() == 0.f);
        CHECK(a.Z() == 0.f);

        Vector<3> b{ 1.f, 2.f, 3.f };
        CHECK(b.X() == 1.f);
        CHECK(b.Y() == 2.f);
        CHECK(b.Z() == 3.f);

        Vector<3> c(2.f);
        CHECK(c.X() == 2.f);
        CHECK(c.Y() == 2.f);
        CHECK(c.Z() == 2.f);
    }

    SECTION("Copy ctor") {
        Vector<3> a{ 1.f, 2.f, 3.f };
        Vector<3> b(a);
        CHECK(b.X() == 1.f);
        CHECK(b.Y() == 2.f);
        CHECK(b.Z() == 3.f);
        Vector<3> c(Vector<3>{ 4.f, 5.f, 6.f });
        CHECK(c.X() == 4.f);
        CHECK(c.Y() == 5.f);
        CHECK(c.Z() == 6.f);
    }

    SECTION("Copy assignment") {
        Vector<3> a{ 1.f, 2.f, 3.f };
        Vector<3> b;
        b = a;
        CHECK(b.X() == 1.f);
        CHECK(b.Y() == 2.f);
        CHECK(b.Z() == 3.f);
        b = Vector<3>{ 7.f, 8.f, 9.f };
        CHECK(b.X() == 7.f);
        CHECK(b.Y() == 8.f);
        CHECK(b.Z() == 9.f);
    }

    SECTION("Vector arithmetic operators") {
        Vector<3> a{ 1.f, 2.f, 3.f };
        Vector<3> b = a + a;
        CHECK(b == Vector<3>{ 2.f, 4.f, 6.f });
        CHECK(a == Vector<3>{ 1.f, 2.f, 3.f });
        Vector<3> c = b - a;
        CHECK(c == a);
        CHECK(b == Vector<3>{ 2.f, 4.f, 6.f });
    }

    SECTION("Magnitude") {
        CHECK(Vector<3>().Magnitude() == 0.f);
        Vector<3> a{ 1.f, 2.f, 3.f };
        CHECK(a.Magnitude() == std::sqrt(14.f));
        Vector<3> b{ 3.f, 2.f, 1.f };
        CHECK(b.Magnitude() == a.Magnitude());

        auto mag = [](float x, float y, float z) {
            return std::sqrt(x * x + y * y + z * z);
        };

        for (unsigned i = 0; i < 100; ++i) {
            float x = RandomFloat();
            float y = RandomFloat();
            float z = RandomFloat();
            REQUIRE(Vector<3>{ x, y, z }.Magnitude() == mag(x, y, z));
        }
    }

    SECTION("Normalization") {
        CHECK(Vector<3>().Normalize() == Vector<3>());

        Vector<3> a{ 2.f, 1.f, 6.f };
        Vector<3> b = Vector<3>::Normalized(a);
        CHECK(a.Normalize() == b);

        auto norm = [](float x, float y, float z) {
            float mag = std::sqrt(x * x + y * y + z * z);
            if (mag == 0.f)
                return Vector<3>();
            return Vector<3>{ x / mag, y / mag, z / mag };
        };

        for (unsigned i = 0; i < 100; ++i) {
            float x = RandomFloat();
            float y = RandomFloat();
            float z = RandomFloat();
            REQUIRE(Vector<3>{ x, y, z }.Normalize() == norm(x, y, z));
        }
    }

    SECTION("Invert") {
        CHECK(Vector<3>().Invert() == Vector<3>());

        Vector<3> a{ 1.f, 2.f, 3.f };
        Vector<3> b{ -1.f, -2.f, -3.f };
        CHECK(a.Invert() == b);

        auto inv = [](float x, float y, float z) {
            return Vector<3>{ -x, -y, -z };
        };

        for (unsigned i = 0; i < 100; ++i) {
            float x = RandomFloat();
            float y = RandomFloat();
            float z = RandomFloat();
            CHECK(Vector<3>{ x, y, z }.Invert() == inv(x, y, z));
        }
    }

    SECTION("Dot product") {
        CHECK(Vector<3>::Dot(Vector<3>(), Vector<3>()) == 0);
        CHECK(Vector<3>::Dot(Vector<3>(1.f), Vector<3>(1.f)) == 3.f);

        auto dot = [](const Vector<3>& lhs, const Vector<3>& rhs) {
            return lhs.X() * rhs.X() + lhs.Y() * rhs.Y() + lhs.Z() * rhs.Z();
        };

        for (unsigned i = 0; i < 100; ++i) {
            float x = RandomFloat();
            float y = RandomFloat();
            float z = RandomFloat();
            float p = RandomFloat();
            float q = RandomFloat();
            float r = RandomFloat();
            CHECK(Vector<3>::Dot({ x, y, z }, { p, q, r }) == dot({ x, y, z }, { p, q, r }));
        }
    }

    SECTION("Cross product") {
        CHECK(Vector<3>::Cross(Vector<3>(), Vector<3>()) == Vector<3>());
        CHECK(Vector<3>::Cross({ 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }) == Vector<3>{ 0.f, 0.f, 1.f });

        auto cross = [](const Vector<3>& lhs, const Vector<3>& rhs) {
            return Vector<3>{ lhs.Y() * rhs.Z() - rhs.Y() * lhs.Z(), lhs.Z() * rhs.X() - rhs.Z() * lhs.X(), lhs.X() * rhs.Y() - rhs.X() * lhs.Y() };
        };

        for (unsigned i = 0; i < 100; ++i) {
            float x = RandomFloat();
            float y = RandomFloat();
            float z = RandomFloat();
            float p = RandomFloat();
            float q = RandomFloat();
            float r = RandomFloat();
            CHECK(Vector<3>::Cross({ x, y, z }, { p, q, r }) == cross({ x, y, z }, { p, q, r }));
        }
    }

    SECTION("Matrix multiplication") {
        Vector<3> vec = { 1.f, 2.f, 3.f };
        Matrix<3> mat = {
            1.f, 2.f, 3.f,
            4.f, 5.f, 6.f,
            7.f, 8.f, 9.f
        };

        Vector<3> expected = { 14.f, 32.f, 50.f };
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
            REQUIRE(Vector<3>{ x, y, z }.Translate({ p, q, r }) == Vector<3>{ x, y, z } + Vector<3>{ p, q, r });
            REQUIRE_APPROX(Vector<3>{ x, y, z }.Rotate(angle, { p, q, r }), ToVector(glm::rotate(glm::vec3(x, y, z), angle, glm::vec3(p, q, r))));
            REQUIRE(Vector<3>{ x, y, z }.Scale({ p, q, r }) == Vector<3>{ x, y, z } * Vector<3>{ p, q, r });
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
        auto myMat = Perspective(45.f, 16.f / 9.f, 1.f, 100.f);
        glm::mat4 glmMat = glm::perspective(glm::radians(45.f), 16.f / 9.f, 1.f, 100.f);
        std::stringstream ss;
        ss << glmMat;
        INFO(ss.str());
        CHECK(myMat == glmMat);
    }

    SECTION("Ortho") {
        auto myMat = Ortho(0.f, 16.f, 0.f, 9.f, 1.f, 100.f);
        glm::mat4 glmMat = glm::ortho(0.f, 16.f, 0.f, 9.f, 1.f, 100.f);
        std::stringstream ss;
        ss << glmMat;
        INFO(ss.str());
        CHECK(myMat == glmMat);
    }
}

TEST_CASE("Utility") {
    SECTION("Radians") {
        CHECK(Radians(10.f) == Approx(glm::radians(10.f)).epsilon(0.0001f));
        float angle = 150.f;
        CHECK(Radians(angle) == Approx(glm::radians(angle)).epsilon(0.0001f));
    }
}