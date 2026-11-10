#ifndef AABB_H
#define AABB_H

#include "vec3.h"
#include "ray.h"
#include <algorithm>

class aabb {
public:
    point3 min;
    point3 max;

    aabb() {}
    aabb(const point3& a, const point3& b) : min(a), max(b) {}

    // Slab method for ray-box intersection
    bool hit(const ray& r, double ray_tmin, double ray_tmax) const {
        for (int a = 0; a < 3; a++) {
            auto invD = 1.0 / r.direction()[a];
            auto t0 = (min[a] - r.origin()[a]) * invD;
            auto t1 = (max[a] - r.origin()[a]) * invD;

            if (invD < 0.0) std::swap(t0, t1);

            ray_tmin = t0 > ray_tmin ? t0 : ray_tmin;
            ray_tmax = t1 < ray_tmax ? t1 : ray_tmax;

            if (ray_tmax <= ray_tmin)
                return false;
        }
        return true;
    }
};

// Optional helper to combine two bounding boxes (note the () on .x(), .y(), .z())
inline aabb surrounding_box(const aabb& box0, const aabb& box1) {
    point3 small(fmin(box0.min.x(), box1.min.x()),
                 fmin(box0.min.y(), box1.min.y()),
                 fmin(box0.min.z(), box1.min.z()));
    point3 big(fmax(box0.max.x(), box1.max.x()),
               fmax(box0.max.y(), box1.max.y()),
               fmax(box0.max.z(), box1.max.z()));
    return aabb(small, big);
}

#endif