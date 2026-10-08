#pragma once
#include <memory>
#include "vec3.h"
#include "ray.h"

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
};