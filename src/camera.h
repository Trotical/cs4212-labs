#pragma once

#include <cmath>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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

class PerspectiveCamera : public Camera {
public:
    // Public fields expected by SceneLoader
    std::string type = "perspective";
    glm::vec3 origin{0.0f, 0.0f, 0.0f};
    glm::vec3 viewDir{0.0f, 0.0f, -1.0f};
    glm::vec3 upDir{0.0f, 1.0f, 0.0f};
    float focal_length = 1.0f;
    float plane_width  = 2.0f;

    int imageWidth  = 500;
    int imageHeight = 500;

    PerspectiveCamera() = default;

    void updateBasis() {
        position = origin;
        glm::vec3 look = glm::normalize(viewDir);
        w = -look; // Backward (-Look)
        u = glm::normalize(glm::cross(look, upDir)); // Right
        v = glm::cross(u, look); // Up

        focalLength = focal_length;
        filmWidth   = plane_width;
        float aspect = static_cast<float>(imageHeight) / static_cast<float>(imageWidth);
        filmHeight  = filmWidth * aspect;
    }

    glm::mat4 getViewMatrix() const {
        glm::vec3 look = glm::normalize(viewDir);
        glm::vec3 right = glm::normalize(glm::cross(look, upDir));
        glm::vec3 up = glm::cross(right, look);
        return glm::lookAt(origin, origin + look, up);
    }

    glm::mat4 getPerspectiveMatrix(float aspectRatio, float zNear = 0.1f, float zFar = 100.0f) const {
        float halfHeight = (plane_width / aspectRatio) * 0.5f;
        float fovY = 2.0f * std::atan2(halfHeight, focal_length);
        return glm::perspective(glm::radians(60.0f), aspectRatio, zNear, zFar);
    }

    // Standard pinhole perspective ray generation from pixel (i, j)
    ray generateRay(int i, int j) override {
        updateBasis();

        // Map pixel center (i + 0.5, j + 0.5) to image plane coordinates
        float u_coord = -filmWidth  * 0.5f + filmWidth  * (i + 0.5f) / static_cast<float>(imageWidth);
        float v_coord = -filmHeight * 0.5f + filmHeight * (j + 0.5f) / static_cast<float>(imageHeight);

        glm::vec3 rayDir = glm::normalize(-focalLength * w + u_coord * u + v_coord * v);

        // Convert glm::vec3 to your custom point3 / vec3 types
        point3 rayOrigin(position.x, position.y, position.z);
        vec3 direction(rayDir.x, rayDir.y, rayDir.z);

        return ray(rayOrigin, direction);
    }
};