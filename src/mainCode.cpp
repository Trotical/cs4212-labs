#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>
#include <cstdlib>  
#include <iostream> 
#include <chrono>

#include "vec3.h"
#include "ray.h"
#include "shape.h"
#include "shader.h"
#include "sphere.h"
#include "triangle.h"
#include "framebuffer.h"
#include "bvh.h"
#include "SceneLoader.h"
#include "SceneParser_JSON.h"
#include "camera.h"

// Recursive scene traversal function supporting reflections, lights, and shadows
// Recursive scene traversal function supporting BVH acceleration and shaders
color traceRay(
    const ray& r, 
    const std::vector<std::shared_ptr<Shape>>& world, 
    const std::vector<PointLight>& lights, 
    const color& background, 
    int depth
) {
    if (depth <= 0) return color(0, 0, 0);

    HitRecord closestHit;
    closestHit.t = 1e9;
    bool hitAnything = false;

    for (const auto& shape : world) {
        HitRecord rec;
        if (shape->intersect(r, rec)) {
            if (rec.t < closestHit.t) {
                closestHit = rec;
                hitAnything = true;
            }
        }
    }

    if (hitAnything && closestHit.shader) {
        return closestHit.shader->rayColor(closestHit, lights, r.direction(), depth, world, background);
    }

    return background;
}

color traceRay(
    const ray& r, 
    const std::shared_ptr<Shape>& world_root,             // BVH root for fast ray intersection
    const std::vector<std::shared_ptr<Shape>>& world,     // Flat vector for shaders (shadows/reflections)
    const std::vector<PointLight>& lights, 
    const color& background, 
    int depth
) {
    if (depth <= 0) return color(0, 0, 0);

    HitRecord closestHit;
    closestHit.t = 1e9; // Initialize to max distance

    // Fast BVH intersection test
    if (world_root && world_root->intersect(r, closestHit)) {
        if (closestHit.shader) {
            return closestHit.shader->rayColor(closestHit, lights, r.direction(), depth, world, background);
        }
    }

    return background;
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " <width> <height> <output.png> <scene.json>\n";
        return 1;
    }

    int image_width = std::stoi(argv[1]);
    int image_height = std::stoi(argv[2]);
    std::string output_filename = argv[3];
    std::string scene_filename = argv[4];

    std::cout << "Loading JSON scene: " << scene_filename 
              << " (" << image_width << "x" << image_height << " -> " << output_filename << ")\n";

    // 1. Parse JSON into SceneContainer
    SceneContainer scene;
    auto loader = std::make_shared<SceneLoader>(scene);
    SceneParser_JSON parser(loader);
    parser.parseFileData(scene_filename);

    if (scene.cameras.empty()) {
        std::cerr << "Error: No cameras found in " << scene_filename << "\n";
        return 1;
    }

    // 2. Extract Camera Parameters
    auto cam = std::dynamic_pointer_cast<PerspectiveCamera>(scene.cameras[0]);
    if (!cam) {
        std::cerr << "Error: Camera 0 is not a PerspectiveCamera.\n";
        return 1;
    }

    point3 cameraOrigin(cam->origin.x, cam->origin.y, cam->origin.z);
    vec3 viewDir = unit_vector(vec3(cam->viewDir.x, cam->viewDir.y, cam->viewDir.z));
    double focal_length = cam->focal_length;
    double plane_width = cam->plane_width;
    double plane_height = plane_width * (static_cast<double>(image_height) / image_width);

    vec3 worldUp(0.0, 1.0, 0.0);
    vec3 u_cam = unit_vector(cross(viewDir, worldUp));
    vec3 v_cam = cross(u_cam, viewDir);

    // 3. Build BVH from loaded shapes
    std::vector<std::shared_ptr<Shape>>& world = scene.shapes;
    std::vector<PointLight>& lights = scene.lights;
    color backgroundColor(0.1, 0.1, 0.15);

    auto bvh_root = std::make_shared<bvh_node>(world, 0, world.size());

    std::cout << "BVH built successfully (" << world.size() << " shapes, " 
              << lights.size() << " lights). Starting render timer...\n";
    auto start_time = std::chrono::high_resolution_clock::now();

    // 4. Render Loop
    const int max_depth = 5;
    Framebuffer fb(image_width, image_height);

    for (int y = 0; y < image_height; ++y) {
        for (int x = 0; x < image_width; ++x) {
            color pixelColor(0, 0, 0);
            const int samples = 10;

            for (int sy = 0; sy < samples; ++sy) {
                for (int sx = 0; sx < samples; ++sx) {
                    double sub_x = (sx + ((double)rand() / RAND_MAX)) / samples;
                    double sub_y = (sy + ((double)rand() / RAND_MAX)) / samples;

                    double px = ((x + sub_x) / image_width) - 0.5;
                    double py = 0.5 - ((y + sub_y) / image_height);

                    vec3 rayDir = unit_vector(focal_length * viewDir + (px * plane_width) * u_cam + (py * plane_height) * v_cam);
                    ray r(cameraOrigin, rayDir);

                    pixelColor += traceRay(r, bvh_root, world, lights, backgroundColor, max_depth);
                }
            }

            pixelColor = pixelColor * (1.0 / (samples * samples));
            fb.setPixelColor(x, y, pixelColor);
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    std::cout << "Render complete in " << elapsed.count() << " seconds. Exported to " << output_filename << std::endl;

    fb.exportToPNG(output_filename);
    return 0;
}