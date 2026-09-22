#pragma once

#include <glm/glm.hpp>
#include "ray.h"

class Camera {
public:
    virtual ~Camera() = default;

    // Generates a ray for screen/image pixel coordinates (i, j)
    virtual ray generateRay(int i, int j) = 0;

protected:
    // Common camera parameters discussed in class
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    
    // Camera orthonormal basis vectors
    glm::vec3 u{1.0f, 0.0f, 0.0f}; // Right
    glm::vec3 v{0.0f, 1.0f, 0.0f}; // Up
    glm::vec3 w{0.0f, 0.0f, 1.0f}; // Backward (-Look)

    // Image plane parameters
    float focalLength = 1.0f;
    float filmWidth   = 2.0f;
    float filmHeight  = 2.0f;
};