#pragma once
#include "vec3.h"
#include "ray.h"

struct HitRecord {
    double t;
    point3 p;
    vec3 normal;
};

class Shape {
public:
    virtual ~Shape() = default;
    virtual bool intersect(const ray& r, HitRecord& rec) const = 0;
};