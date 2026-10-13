#pragma once
#include <cmath>
#include "shape.h"

class Triangle : public Shape {
public:
    point3 a, b, c;

    Triangle(const point3& p0, const point3& p1, const point3& p2, std::shared_ptr<Shader> s)
        : Shape(s), a(p0), b(p1), c(p2) {}

    bool intersect(const ray& r, HitRecord& rec) const override {
        // Möller–Trumbore ray-triangle intersection algorithm
        vec3 edge1 = b - a;
        vec3 edge2 = c - a;
        vec3 h = cross(r.direction(), edge2);
        double det = dot(edge1, h);

        // Parallel check
        if (std::abs(det) < 1e-8) return false;

        double invDet = 1.0 / det;
        vec3 s = r.origin() - a;
        double u = invDet * dot(s, h);
        if (u < 0.0 || u > 1.0) return false;

        vec3 q = cross(s, edge1);
        double v = invDet * dot(r.direction(), q);
        if (v < 0.0 || u + v > 1.0) return false;

        double t = invDet * dot(edge2, q);
        if (t < 0.001) return false; // Ignore intersections behind camera or too close

        rec.t = t;
        rec.p = r.at(t);
        rec.normal = unit_vector(cross(edge1, edge2)); // Flat face normal
        rec.shader = shader;

        return true;
    }
};