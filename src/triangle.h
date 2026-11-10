#pragma once
#include <cmath>
#include <algorithm>
#include "shape.h"
#include "aabb.h"

class Triangle : public Shape {
public:
    point3 a, b, c;

    Triangle(const point3& p0, const point3& p1, const point3& p2, std::shared_ptr<Shader> s)
        : Shape(s), a(p0), b(p1), c(p2) {}

    bool intersect(const ray& r, HitRecord& rec) const override {
        vec3 edge1 = b - a;
        vec3 edge2 = c - a;
        vec3 h = cross(r.direction(), edge2);
        double det = dot(edge1, h);

        // Parallel check
        if (std::abs(det) < 1e-8) return false;

        double invDet = 1.0 / det;
        vec3 s_vec = r.origin() - a;
        double u = invDet * dot(s_vec, h);
        if (u < 0.0 || u > 1.0) return false;

        vec3 q = cross(s_vec, edge1);
        double v = invDet * dot(r.direction(), q);
        if (v < 0.0 || u + v > 1.0) return false;

        double t = invDet * dot(edge2, q);
        if (t < 0.001) return false;

        rec.t = t;
        rec.p = r.at(t);
        rec.normal = unit_vector(cross(edge1, edge2));
        rec.shader = shader;

        return true;
    }
    
    // Bounding box implementation using accessor methods x(), y(), z()
    bool bounding_box(aabb& output_box) const override {
        double min_x = a.x(); if (b.x() < min_x) min_x = b.x(); if (c.x() < min_x) min_x = c.x();
        double min_y = a.y(); if (b.y() < min_y) min_y = b.y(); if (c.y() < min_y) min_y = c.y();
        double min_z = a.z(); if (b.z() < min_z) min_z = b.z(); if (c.z() < min_z) min_z = c.z();

        double max_x = a.x(); if (b.x() > max_x) max_x = b.x(); if (c.x() > max_x) max_x = c.x();
        double max_y = a.y(); if (b.y() > max_y) max_y = b.y(); if (c.y() > max_y) max_y = c.y();
        double max_z = a.z(); if (b.z() > max_z) max_z = b.z(); if (c.z() > max_z) max_z = c.z();

        point3 min_vec(min_x, min_y, min_z);
        point3 max_vec(max_x, max_y, max_z);

        double pad = 0.0001;
        output_box = aabb(
            min_vec - vec3(pad, pad, pad),
            max_vec + vec3(pad, pad, pad)
        );

        return true;
    }
};