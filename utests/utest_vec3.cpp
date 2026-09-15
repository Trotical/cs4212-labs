#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "vec3.h"

constexpr double EPSILON = 1.0e-6;

TEST_CASE("Vector Addition and Subtraction Operations", "[vec3][math]") {
    vec3 a(1.0, 2.0, 3.0);
    vec3 b(4.0, 5.0, 6.0);

    vec3 sum = a + b;
    REQUIRE_THAT(sum.x(), Catch::Matchers::WithinAbs(5.0, EPSILON));
    REQUIRE_THAT(sum.y(), Catch::Matchers::WithinAbs(7.0, EPSILON));
    REQUIRE_THAT(sum.z(), Catch::Matchers::WithinAbs(9.0, EPSILON));

    vec3 diff = a - b;
    REQUIRE_THAT(diff.x(), Catch::Matchers::WithinAbs(-3.0, EPSILON));
    REQUIRE_THAT(diff.y(), Catch::Matchers::WithinAbs(-3.0, EPSILON));
    REQUIRE_THAT(diff.z(), Catch::Matchers::WithinAbs(-3.0, EPSILON));
}

TEST_CASE("Scalar Multiplication and Division Scaling", "[vec3][math]") {
    vec3 v(2.0, -4.0, 8.0);

    vec3 scaled = v * 0.5;
    REQUIRE_THAT(scaled.x(), Catch::Matchers::WithinAbs(1.0, EPSILON));
    REQUIRE_THAT(scaled.y(), Catch::Matchers::WithinAbs(-2.0, EPSILON));
    REQUIRE_THAT(scaled.z(), Catch::Matchers::WithinAbs(4.0, EPSILON));

    vec3 divided = v / 2.0;
    REQUIRE_THAT(divided.x(), Catch::Matchers::WithinAbs(1.0, EPSILON));
    REQUIRE_THAT(divided.y(), Catch::Matchers::WithinAbs(-2.0, EPSILON));
    REQUIRE_THAT(divided.z(), Catch::Matchers::WithinAbs(4.0, EPSILON));
}

TEST_CASE("Length and Magnitude Computations", "[vec3][geometry]") {
    vec3 v(3.0, 4.0, 12.0);

    REQUIRE_THAT(v.length_squared(), Catch::Matchers::WithinAbs(169.0, EPSILON));
    REQUIRE_THAT(v.length(), Catch::Matchers::WithinAbs(13.0, EPSILON));
}

TEST_CASE("Dot Product Behavior and Orthogonality", "[vec3][linear_algebra]") {
    vec3 posX(1.0, 0.0, 0.0);
    vec3 posY(0.0, 1.0, 0.0);

    REQUIRE_THAT(dot(posX, posY), Catch::Matchers::WithinAbs(0.0, EPSILON));

    vec3 v(2.0, 3.0, 4.0);
    REQUIRE_THAT(dot(v, v), Catch::Matchers::WithinAbs(v.length_squared(), EPSILON));
}

TEST_CASE("Cross Product and Vector Normalization", "[vec3][linear_algebra]") {
    vec3 i(1.0, 0.0, 0.0);
    vec3 j(0.0, 1.0, 0.0);

    vec3 k = cross(i, j);
    REQUIRE_THAT(k.x(), Catch::Matchers::WithinAbs(0.0, EPSILON));
    REQUIRE_THAT(k.y(), Catch::Matchers::WithinAbs(0.0, EPSILON));
    REQUIRE_THAT(k.z(), Catch::Matchers::WithinAbs(1.0, EPSILON));

    vec3 v(5.0, -2.0, 10.0);
    vec3 u = unit_vector(v);
    REQUIRE_THAT(u.length(), Catch::Matchers::WithinAbs(1.0, EPSILON));
}