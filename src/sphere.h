#pragma once
#include "shape.h"
#include "vec3.h"
#include "ray.h"
#include <cmath>

class Sphere : public Shape {
public:
    point3 center;
    double radius;

    Sphere(const point3& c, double r) : center(c), radius(r) {}

    bool intersect(const ray& r, HitRecord& rec) const override {
        vec3 oc = r.origin() - center;
        
        double a = dot(r.direction(), r.direction());
        double b = 2.0 * dot(oc, r.direction());
        double c = dot(oc, oc) - (radius * radius);

        double discriminant = (b * b) - (4.0 * a * c);

        if (discriminant < 0.0) {
            return false;
        }

        
        double sqrtd = std::sqrt(discriminant);
        double root = (-b - sqrtd) / (2.0 * a);
        
        if (root < 0.001) {
            root = (-b + sqrtd) / (2.0 * a);
            if (root < 0.001) {
                return false;
            }
        }

        
        rec.t = root;
        rec.p = r.at(rec.t);
        rec.normal = unit_vector(rec.p - center); // Outward unit normal

        return true;
    }
};