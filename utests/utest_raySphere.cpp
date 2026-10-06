#include "vec3.h"
#include "sphere.h"
#include "ray.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
TEST_CASE("Ray-Sphere Intersection Tests", "[sphere]") {
    Sphere sphere(vec3(0.0f, 0.0f, -5.0f), 2.0f); // sphere at z = -5 with radius 2

    SECTION("Direct Hit through center") {
        ray ray(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
        REQUIRE(sphere.intersect(ray) == true);
    }

    SECTION("A Miss (Ray points away or past sphere)") {
        ray ray(vec3(0.0f, 10.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
        REQUIRE(sphere.intersect(ray) == false);
    }

    SECTION("A Tangent Hit (Glancing edge)") {
        ray ray(vec3(2.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
        REQUIRE(sphere.intersect(ray) == true);
    }
}