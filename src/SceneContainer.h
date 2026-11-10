#pragma once

#include <vector>
#include <memory>
#include "shape.h"
#include "light.h"
#include "vec3.h"
#include "camera.h"
#include "bvh.h" // 

struct SceneContainer {
    std::vector<std::shared_ptr<Shape>> shapes;
    std::vector<PointLight> lights;
    std::vector<std::shared_ptr<Camera>> cameras;
    
    color backgroundColor = color(0.0, 0.0, 0.0);

    // 2. Store the root of your acceleration structure
    std::shared_ptr<Shape> bvh_root = nullptr;

    // 3. Call this AFTER you finish loading/adding all your shapes
    void buildBVH() {
        if (!shapes.empty()) {
            bvh_root = std::make_shared<bvh_node>(shapes, 0, shapes.size());
        } else {
            bvh_root = nullptr;
        }
    }
};