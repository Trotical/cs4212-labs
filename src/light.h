#pragma once

#include "vec3.h"

class PointLight {
public:
    point3 position;
    color intensity;

    // Default constructor
    PointLight() 
        : position(0.0, 0.0, 0.0), intensity(1.0, 1.0, 1.0) {}

    // Parameterized constructor
    PointLight(const point3& pos, const color& intens) 
        : position(pos), intensity(intens) {}
};