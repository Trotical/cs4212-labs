#pragma once

#include "vec3.h"

class ray {
public:
    // Default constructor
    ray() : orig(point3()), dir(vec3()) {}

    // Constructor with origin and direction
    ray(const point3& origin, const vec3& direction)
        : orig(origin), dir(direction) {}

    // Const accessors
    [[nodiscard]] point3 origin() const { return orig; }
    [[nodiscard]] vec3 direction() const { return dir; }

    // Parametric evaluation: P(t) = A + t*B
    [[nodiscard]] point3 at(double t) const {
        return orig + t * dir;
    }

private:
    point3 orig;
    vec3 dir;
};