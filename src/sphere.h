#pragma once
#include <memory>
#include <cmath>
#include "shape.h"
#include "shader.h"

class Sphere : public Shape {
public:
    point3 center;
    double radius;

    // Constructor MUST accept std::shared_ptr<Shader> as the 3rd parameter
    Sphere(const point3& c, double r, std::shared_ptr<Shader> s = nullptr)
        : Shape(s), center(c), radius(r) {}

    bool intersect(const ray& r, HitRecord& rec) const override {
        vec3 oc = r.origin() - center;
        
        double a = dot(r.direction(), r.direction());
        double b = 2.0 * dot(oc, r.direction());
        double c = dot(oc, oc) - (radius * radius);

        double discriminant = (b * b) - (4.0 * a * c);

        if (discriminant < 0.0) return false;

        double sqrtd = std::sqrt(discriminant);
        double root = (-b - sqrtd) / (2.0 * a);
        
        if (root < 0.001) {
            root = (-b + sqrtd) / (2.0 * a);
            if (root < 0.001) return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        rec.normal = unit_vector(rec.p - center);
        rec.shader = shader; // passes attached shader to hit record

        return true;
    }
};