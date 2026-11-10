#pragma once
#include <memory>
#include "vec3.h"
#include "ray.h"
#include "aabb.h" // 1. Include your new AABB header

// forward declaration tells C++ Shader is a class
class Shader;

struct HitRecord {
    double t;
    point3 p;
    vec3 normal;
    std::shared_ptr<Shader> shader; // smart pointer to attached shader
};

class Shape {
public:
    std::shared_ptr<Shader> shader;

    Shape(std::shared_ptr<Shader> s = nullptr) : shader(s) {}
    virtual ~Shape() = default;

    virtual bool intersect(const ray& r, HitRecord& rec) const = 0;
    virtual bool bounding_box(aabb& output_box) const = 0;
};