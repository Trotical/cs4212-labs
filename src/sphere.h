#pragma once
#include "shape.h"
#include "vec3.h"
#include "ray.h"

class Sphere : public Shape {
public:
    point3 center;
    double radius;

    Sphere(const point3& c, double r) : center(c), radius(r) {}

    bool intersect(const ray& r) const override {
        vec3 oc = r.origin() - center;
        
        double a = dot(r.direction(), r.direction());
        double b = 2.0 * dot(oc, r.direction());
        double c = dot(oc, oc) - (radius * radius);

        double discriminant = (b * b) - (4.0 * a * c);

        // ray hits the sphere if discriminant is non-negative
        return discriminant >= 0.0;
    }
};