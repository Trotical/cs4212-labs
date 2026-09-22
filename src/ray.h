#pragma once

#include <glm/glm.hpp>

class ray {
public:
    using point3 = glm::vec3;
    using vec3   = glm::vec3;

    // Default constructor
    ray() : orig(0.0f), dir(0.0f) {}

    // Constructor with origin and direction
    ray(const point3& origin, const vec3& direction)
        : orig(origin), dir(direction) {}

    // Const accessors
    [[nodiscard]] point3 origin() const { return orig; }
    [[nodiscard]] vec3 direction() const { return dir; }

    // Parametric evaluation: P(t) = A + t*B
    [[nodiscard]] point3 at(float t) const {
        return orig + t * dir;
    }

private:
    point3 orig;
    vec3 dir;
};