#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>
#include <cstdlib>

#include "vec3.h"
#include "ray.h"
#include "shape.h"
#include "shader.h"
#include "sphere.h"
#include "triangle.h"
#include "framebuffer.h"

// Recursive scene traversal function
color traceRay(
    const ray& r, 
    const std::vector<std::shared_ptr<Shape>>& world, 
    const std::vector<PointLight>& lights, 
    const color& background, 
    int depth
) {
    if (depth <= 0) return color(0, 0, 0);

    HitRecord closestHit;
    bool hitAnything = false;
    double closestSoFar = 1e9;

    for (const auto& shape : world) {
        HitRecord rec;
        if (shape->intersect(r, rec)) {
            if (rec.t < closestSoFar) {
                closestSoFar = rec.t;
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

// Inside main() in src/mainCode.cpp

std::vector<PointLight> lights = {
    // Light1 (upper left)
    { point3(-5.0, 8.0, 5.0), color(0.7, 0.6, 0.45) },
    
    // Light2 (upper right)
    { point3(6.0, 6.0, 2.0),  color(0.35, 0.45, 0.7) }
};

int main() {
    const int image_width = 500;
    const int image_height = 500;
    const int max_depth = 5;
    Framebuffer fb(image_width, image_height);

    point3 cameraOrigin(0.0, 3.0, 3.0);
    color backgroundColor(0.5, 0.7, 1.0);

    std::vector<PointLight> lights = {
        { point3(-5.0, 8.0, 5.0), color(0.7, 0.6, 0.45) },
        { point3(6.0, 6.0, 2.0),  color(0.35, 0.45, 0.7) }
    };

    auto greyLambert   = std::make_shared<Lambertian>(color(0.6, 0.6, 0.6));
    auto yellowLambert = std::make_shared<Lambertian>(color(0.9, 0.8, 0.1));
    auto greenLambert  = std::make_shared<Lambertian>(color(0.2, 0.8, 0.2));
    auto tealLambert   = std::make_shared<Lambertian>(color(0.2, 0.7, 0.7));
    auto purpleLambert = std::make_shared<Lambertian>(color(0.6, 0.2, 0.8));

    auto redPhong      = std::make_shared<BlinnPhong>(color(0.8, 0.1, 0.1), 64.0);
    auto bluePhong     = std::make_shared<BlinnPhong>(color(0.1, 0.2, 0.8), 16.0);
    auto orangePhong   = std::make_shared<BlinnPhong>(color(0.95, 0.5, 0.1), 32.0);

    auto normalShader  = std::make_shared<NormalShader>();
    auto mirrorShader  = std::make_shared<MirrorShader>();

    std::vector<std::shared_ptr<Shape>> world;

    // Ground
    world.push_back(std::make_shared<Triangle>(point3(-50,0,50), point3(50,0,50), point3(50,0,-50), greyLambert));
    world.push_back(std::make_shared<Triangle>(point3(-50,0,50), point3(50,0,-50), point3(-50,0,-50), greyLambert));

    // Walls (Mirror)
    world.push_back(std::make_shared<Triangle>(point3(-4,0,-6), point3(4,0,-6), point3(4,4,-6), mirrorShader));
    world.push_back(std::make_shared<Triangle>(point3(-4,0,-6), point3(4,4,-6), point3(-4,4,-6), mirrorShader));

    // Floating Triangle
    world.push_back(std::make_shared<Triangle>(point3(0.9,1.4,-0.6), point3(1.9,1.4,-0.6), point3(1.4,2.2,-1.1), yellowLambert));

    // Spheres
    world.push_back(std::make_shared<Sphere>(point3(-2.0, 0.8, -3.0), 0.8, greenLambert));
    world.push_back(std::make_shared<Sphere>(point3(-1.1, 0.6, -1.2), 0.6, redPhong));
    world.push_back(std::make_shared<Sphere>(point3(0.0, 0.9, -3.5), 0.9, normalShader));
    world.push_back(std::make_shared<Sphere>(point3(0.3, 0.45, -1.6), 0.45, bluePhong));
    world.push_back(std::make_shared<Sphere>(point3(2.0, 1.0, -3.0), 1.0, mirrorShader));
    world.push_back(std::make_shared<Sphere>(point3(1.5, 0.5, 0.3), 0.5, mirrorShader));
    world.push_back(std::make_shared<Sphere>(point3(-1.3, 0.45, 0.4), 0.45, tealLambert));

    // Behind camera
    world.push_back(std::make_shared<Sphere>(point3(0.3, 1.0, 5.0), 1.0, orangePhong));
    world.push_back(std::make_shared<Sphere>(point3(2.4, 0.8, 4.8), 0.8, purpleLambert));

    // Pixel render loop with 10x10 jittered sampling
    for (int y = 0; y < image_height; ++y) {
        for (int x = 0; x < image_width; ++x) {
            color pixelColor(0, 0, 0);
            const int samples = 10;

            for (int sy = 0; sy < samples; ++sy) {
                for (int sx = 0; sx < samples; ++sx) {
                    double sub_x = (sx + ((double)rand() / RAND_MAX)) / samples;
                    double sub_y = (sy + ((double)rand() / RAND_MAX)) / samples;

                    double u = (2.0 * (x + sub_x) / image_width) - 1.0;
                    double v = 1.0 - (2.0 * (y + sub_y) / image_height);

                    vec3 direction(u, v - 0.4, -1.0);
                    ray r(cameraOrigin, direction);

                    pixelColor += traceRay(r, world, lights, backgroundColor, max_depth);
                }
            }

            pixelColor = pixelColor * (1.0 / (samples * samples));
            fb.setPixelColor(x, y, pixelColor);
        }
    }

    fb.exportToPNG("scene_output.png");
    std::cout << "Render complete: exported scene_output.png" << std::endl;

    return 0;
}