#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "ray.h"

using Catch::Matchers::WithinAbs;

TEST_CASE("Ray Construction and Accessors", "[ray]") {
    glm::vec3 origin(1.0f, 2.0f, 3.0f);
    glm::vec3 direction(0.0f, 0.0f, -1.0f);

    ray r(origin, direction);

    SECTION("Stores origin and direction accurately") {
        REQUIRE(r.origin() == origin);
        REQUIRE(r.direction() == direction);
    }
}

TEST_CASE("Parametric Line Evaluation", "[ray]") {
    glm::vec3 origin(1.0f, 2.0f, 3.0f);
    glm::vec3 direction(0.0f, 1.0f, 0.0f);
    ray r(origin, direction);

    SECTION("t = 0 returns origin") {
        REQUIRE(r.at(0.0f) == origin);
    }

    SECTION("t = 1 moves 1 unit in direction") {
        REQUIRE(r.at(1.0f) == glm::vec3(1.0f, 3.0f, 3.0f));
    }

    SECTION("Arbitrary positive t") {
        REQUIRE(r.at(5.5f) == glm::vec3(1.0f, 7.5f, 3.0f));
    }

    SECTION("Arbitrary negative t") {
        REQUIRE(r.at(-2.5f) == glm::vec3(1.0f, -0.5f, 3.0f));
    }
}

TEST_CASE("Immutability", "[ray]") {
    const ray r(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    SECTION("at() does not modify ray parameters") {
        [[maybe_unused]] auto p = r.at(10.0f);
        REQUIRE(r.origin() == glm::vec3(0.0f));
        REQUIRE(r.direction() == glm::vec3(1.0f, 0.0f, 0.0f));
    }
}

TEST_CASE("Numerical Robustness", "[ray]") {
    glm::vec3 origin(1.0f / 3.0f, 2.0f / 3.0f, 0.0f);
    glm::vec3 direction(1.0f / 7.0f, 0.0f, 0.0f);
    ray r(origin, direction);

    SECTION("Floating point precision within margin") {
        glm::vec3 p = r.at(7.0f);
        constexpr float margin = 1e-5f;
        REQUIRE_THAT(p.x, WithinAbs(1.3333333f, margin));
        REQUIRE_THAT(p.y, WithinAbs(0.6666666f, margin));
        REQUIRE_THAT(p.z, WithinAbs(0.0f, margin));
    }
}