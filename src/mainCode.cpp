#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

#include "vec3.h"
#include "ray.h"
#include "shape.h"
#include "sphere.h"
#include "framebuffer.h"

// computes Phong Reflection Model (Ambient + Diffuse + Specular)
color phongLighting(
    const point3& hitPoint,
    const vec3& normal,
    const vec3& rayDir,
    const point3& lightPos,
    const color& objectColor,
    double shininess) 
{
    // direction vectors
    vec3 L = unit_vector(lightPos - hitPoint);   // direction toward the light
    vec3 V = unit_vector(-rayDir);               // direction toward the camera

    // ambient component 
    color ambient = 0.1 * objectColor;

    // diffuse component 
    double diff = std::max(0.0, dot(normal, L));
    color diffuse = diff * objectColor;

    // specular component 
    color specular(0.0, 0.0, 0.0);
    if (diff > 0.0 && shininess > 0.0) {
        // flrflection vector R = 2 * (N . L) * N - L
        vec3 R = 2.0 * dot(normal, L) * normal - L;
        double spec = std::pow(std::max(0.0, dot(R, V)), shininess);
        specular = spec * color(1.0, 1.0, 1.0); // White highlight
    }

    // combine all components and clamp values to [0.0, 1.0]
    color finalColor = ambient + diffuse + specular;
    return color(
        std::min(1.0, finalColor.x()),
        std::min(1.0, finalColor.y()),
        std::min(1.0, finalColor.z())
    );
}

int main() {
    // image dimensions
    const int image_width = 512;
    const int image_height = 512;
    Framebuffer fb(image_width, image_height);

    // camera setup
    point3 cameraOrigin(0.4, 0.9, -1.2);

    // point light source (positioned top-left-front)
    point3 lightPos(-3.0, 5.0, 2.0);

    // left pure diffuse (shininess = 0)
    // middle tight, sharp highlight (shininess = 128)
    // right softer, broader highlight (shininess = 16)
    Sphere sphereLeft(point3(-0.1, 0.0, -3.0), 0.5);
    Sphere sphereMid(point3(0.4, 0.7, -3.0), 0.5);
    Sphere sphereRight(point3(0.8, 0.0, -3.0), 0.5);

    // color definitions
    color sphereColor(0.9, 0.8, 0.1);
    color skyBlue(0.7, 0.85, 1.0);
    color floorColor(0.95, 0.95, 0.85);
    color customColor(0.2, 0.4, 6.0);
    color customColor2(0.9, 0.5, 0.8);
    color customColor3(0.9, 0.4, 0.1);

    // render loop over pixels
    for (int y = 0; y < image_height; ++y) {
        for (int x = 0; x < image_width; ++x) {
            // map pixel coordinates to normalized screen space [-1, 1]
            double u = (2.0 * (x + 0.5) / image_width) - 1.0;
            double v = 1.0 - (2.0 * (y + 0.5) / image_height); // Flip Y to match image space

            vec3 direction(u, v - 0.4, -1.0);
            ray r(cameraOrigin, direction);

            HitRecord rec;
            color pixelColor = skyBlue; // default background color

            // simple floor 
            if (v < -0.2) {
                pixelColor = floorColor;
            }

            // ray-sphere intersection checks (renders closest hit)
            if (sphereLeft.intersect(r, rec)) {
                pixelColor = phongLighting(rec.p, rec.normal, r.direction(), lightPos, sphereColor, 0.0);
            } else if (sphereMid.intersect(r, rec)) {
                pixelColor = phongLighting(rec.p, rec.normal, r.direction(), lightPos, customColor, 128.0);
            } else if (sphereRight.intersect(r, rec)) {
                pixelColor = phongLighting(rec.p, rec.normal, r.direction(), lightPos, customColor3, 16.0);
            }

            fb.setPixelColor(x, y, pixelColor);
        }
    }

    // export image output
    fb.exportToPNG("phong_shading.png");
    std::cout << "Successfully rendered scene to phong_shading.png" << std::endl;

    return 0;
}