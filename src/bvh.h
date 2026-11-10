#pragma once

#include <memory>
#include <vector>
#include <algorithm>
#include "shape.h"
#include "aabb.h"

class bvh_node : public Shape {
public:
    std::shared_ptr<Shape> left;
    std::shared_ptr<Shape> right;
    aabb box;

    bvh_node() {}

    // Constructor for building the BVH from a std::vector of shapes
    bvh_node(const std::vector<std::shared_ptr<Shape>>& src_shapes, size_t start, size_t end) {
        auto shapes = src_shapes;

        // Choose a random axis (0 for X, 1 for Y, 2 for Z)
        int axis = rand() % 3;

        size_t object_span = end - start;

        if (object_span == 1) {
            left = right = shapes[start];
        } else if (object_span == 2) {
            // Simple 2-object sort inline
            aabb box_a, box_b;
            shapes[start]->bounding_box(box_a);
            shapes[start + 1]->bounding_box(box_b);
            
            if (box_a.min[axis] < box_b.min[axis]) {
                left = shapes[start];
                right = shapes[start + 1];
            } else {
                left = shapes[start + 1];
                right = shapes[start];
            }
        } else {
            // Sort using a clean lambda expression that captures 'axis'
            std::sort(shapes.begin() + start, shapes.begin() + end, 
                [axis](const std::shared_ptr<Shape>& a, const std::shared_ptr<Shape>& b) {
                    aabb box_a, box_b;
                    if (!a->bounding_box(box_a) || !b->bounding_box(box_b)) {
                        std::cerr << "No bounding box in bvh_node constructor.\n";
                    }
                    return box_a.min[axis] < box_b.min[axis];
                });

            size_t mid = start + object_span / 2;
            left = std::make_shared<bvh_node>(shapes, start, mid);
            right = std::make_shared<bvh_node>(shapes, mid, end);
        }

        aabb box_left, box_right;
        if (!left->bounding_box(box_left) || !right->bounding_box(box_right)) {
            std::cerr << "No bounding box in bvh_node constructor.\n";
        }

        box = surrounding_box(box_left, box_right);
    }

    bool intersect(const ray& r, HitRecord& rec) const override {
        if (!box.hit(r, 0.001, rec.t)) {
            return false;
        }

        bool hit_left = left->intersect(r, rec);
        // If we hit something on the left, pass the updated hit distance (rec.t) 
        // to the right child to clip future farther intersections
        bool hit_right = right->intersect(r, rec);

        return hit_left || hit_right;
    }

    bool bounding_box(aabb& output_box) const override {
        output_box = box;
        return true;
    }

private:
    // Helper comparators for sorting along axes
    static bool box_compare(const std::shared_ptr<Shape>& a, const std::shared_ptr<Shape>& b, int axis) {
        aabb box_a, box_b;
        if (!a->bounding_box(box_a) || !b->bounding_box(box_b)) {
            std::cerr << "No bounding box in bvh_node compare.\n";
        }
        return box_a.min[axis] < box_b.min[axis];
    }

    static bool box_x_compare(const std::shared_ptr<Shape>& a, const std::shared_ptr<Shape>& b) {
        return box_compare(a, b, 0);
    }

    static bool box_y_compare(const std::shared_ptr<Shape>& a, const std::shared_ptr<Shape>& b) {
        return box_compare(a, b, 1);
    }

    static bool box_z_compare(const std::shared_ptr<Shape>& a, const std::shared_ptr<Shape>& b) {
        return box_compare(a, b, 2);
    }

    // Simple random helper if you don't already have one in your utils
    static int random_int(int min, int max) {
        return static_cast<int>(min + (rand() / (RAND_MAX + 1.0)) * (max - min + 1));
    }
};