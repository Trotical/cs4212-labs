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
    int image_width = 500;
    int image_height = 500;
    std::string output_filename = "scene_output2.png";

    if (argc >= 3) {
        image_width = std::stoi(argv[1]);
        image_height = std::stoi(argv[2]);
    }
    if (argc >= 4) {
        output_filename = argv[3];
    }

    std::cout << "Rendering custom scene: " << image_width << "x" << image_height 
              << " -> " << output_filename << std::endl;

    const int max_depth = 5;
    Framebuffer fb(image_width, image_height);

    // Camera Setup
    point3 cameraOrigin(0.0, 1.5, 2.5);
    vec3 viewDir = unit_vector(vec3(0.0, 0.15, -1.0));
    double focal_length = 0.4;
    double plane_width = 0.5;
    double plane_height = 0.5;

    vec3 worldUp(0.0, 1.0, 0.0);
    vec3 u_cam = unit_vector(cross(viewDir, worldUp));
    vec3 v_cam = cross(u_cam, viewDir);

    color backgroundColor(0.9, 0.2, 0.4);

    // Point Lights Setup
    std::vector<PointLight> lights = {
        { point3(-5.0, 8.0, 5.0), color(0.7, 0.6, 0.45) },
        { point3(6.0, 6.0, 2.0),  color(0.35, 0.45, 0.7) }
    };

    // Shaders Initialization
    auto greyLambert   = std::make_shared<Lambertian>(color(0.6, 0.6, 0.6));
    auto yellowLambert = std::make_shared<Lambertian>(color(0.0, 0.1, 0.1));
    auto greenLambert  = std::make_shared<Lambertian>(color(0.2, 0.8, 0.2));
    auto redPhong      = std::make_shared<BlinnPhong>(color(0.8, 0.1, 0.1), 64.0);
    auto bluePhong     = std::make_shared<BlinnPhong>(color(0.1, 0.2, 0.8), 16.0);
    auto normalShader  = std::make_shared<NormalShader>();
    auto mirrorShader  = std::make_shared<MirrorShader>();

    // Custom World Geometry Setup
    std::vector<std::shared_ptr<Shape>> world;

    // Overhead Mirror Canopy
    world.push_back(std::make_shared<Triangle>(point3(-4.0, 4.0, -1.0), point3(4.0, 4.0, -1.0), point3(4.0, 3.2, -5.0), mirrorShader));
    world.push_back(std::make_shared<Triangle>(point3(-4.0, 4.0, -1.0), point3(4.0, 3.2, -5.0), point3(-4.0, 3.2, -5.0), mirrorShader));

    // Left Vertical Pillar (Mirror)
    world.push_back(std::make_shared<Triangle>(point3(-3.5, 0.0, -0.5), point3(-3.5, 4.0, -0.5), point3(-3.5, 4.0, -3.5), mirrorShader));
    world.push_back(std::make_shared<Triangle>(point3(-3.5, 0.0, -0.5), point3(-3.5, 4.0, -3.5), point3(-3.5, 0.0, -3.5), mirrorShader));

    // Right Vertical Pillar (Mirror)
    world.push_back(std::make_shared<Triangle>(point3(3.5, 0.0, -0.5), point3(3.5, 4.0, -3.5), point3(3.5, 4.0, -0.5), mirrorShader));
    world.push_back(std::make_shared<Triangle>(point3(3.5, 0.0, -0.5), point3(3.5, 0.0, -3.5), point3(3.5, 4.0, -3.5), mirrorShader));
    
    // Ground Floor
    world.push_back(std::make_shared<Triangle>(point3(-50,0,50), point3(50,0,50), point3(50,0,-50), greyLambert));
    world.push_back(std::make_shared<Triangle>(point3(-50,0,50), point3(50,0,-50), point3(-50,0,-50), greyLambert));

    // Floating Triangle in da center
    world.push_back(std::make_shared<Triangle>(point3(-0.5, 1.5, -2.5), point3(0.5, 1.5, -2.5), point3(0.0, 2.5, -3.0), yellowLambert));

    // Cluster of orbs below the overhead mirror
    world.push_back(std::make_shared<Sphere>(point3(-1.5, 0.6, -2.5), 0.6, greenLambert));   // Left green orb
    world.push_back(std::make_shared<Sphere>(point3( 1.5, 0.6, -2.5), 0.6, redPhong));     // Right red phong orb
    world.push_back(std::make_shared<Sphere>(point3( 0.0, 0.5, -1.8), 0.5, normalShader));    // Front center blue orb
    world.push_back(std::make_shared<Sphere>(point3(-0.8, 1.2, -3.5), 0.8, mirrorShader));  // Back left mirror orb
    world.push_back(std::make_shared<Sphere>(point3( 0.8, 1.2, -3.5), 0.8, mirrorShader));  // Back right normal orb

    auto bvh_root = std::make_shared<bvh_node>(world, 0, world.size());

    std::cout << "BVH built successfully. Starting render timer...\n";
    auto start_time = std::chrono::high_resolution_clock::now();
    // 6. Render Loop
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
                    
                    // this is for BVH usage
                    pixelColor += traceRay(r, bvh_root, world, lights, backgroundColor, max_depth);

                    // this is for linear usage
                    // pixelColor += traceRay(r, world, lights, backgroundColor, max_depth);
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