#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>

#include "vec3.h"
#include "ray.h"
#include "shape.h"
#include "shader.h"
#include "sphere.h"
#include "framebuffer.h"

int main() {
    // I\image dimensions
    const int image_width = 512;
    const int image_height = 512;
    Framebuffer fb(image_width, image_height);

    // camera setup
    point3 cameraOrigin(0.0, 1.2, 1.0);

    // point light source (top-left-front)
    point3 lightPos(-3.0, 5.0, 2.0);

    // material color
    color sphereGreen(0.0, 0.4, 0.1);
    color custom1(0.3, 0.8, 0.9);
    color custom2(0.6, 0.2, 0.7);
    color custom3(0.2, 0.5, 0.9);
    color custom4(0.9, 0.2, 0.2);

    // 1. instantiate shaders using std::make_shared
    std::shared_ptr<Shader> lambertianShader = std::make_shared<Lambertian>(custom3);
    std::shared_ptr<Shader> sharpBlinnPhong  = std::make_shared<BlinnPhong>(custom4, 128.0);
    std::shared_ptr<Shader> softBlinnPhong   = std::make_shared<BlinnPhong>(custom2, 16.0);
   // std::shared_ptr<Shader> background = std::make_shared<Lambertian>(custom1);

    // 2. attach Shaders to Spheres
    Sphere sphereLeft(point3(-0.5, 0.0, -1.0), 0.5, lambertianShader);
    Sphere sphereMid(point3(0.0, 0.8, -1.0), 0.5, sharpBlinnPhong);
    Sphere sphereRight(point3(0.5, 0.0, -1.0), 0.5, softBlinnPhong);
 //   Sphere sphereBack(point3(-0.3, 0.0, -2.0), 0.5, background);

    // background colors
    color white(1.0, 1.0, 1.0);
    color skyBlue(0.5, 0.7, 1.0);
    color floorColor(0.15, 0.15, 0.15);


    // render loop over pixels
    for (int y = 0; y < image_height; ++y) {
        for (int x = 0; x < image_width; ++x) {
            // map pixel coordinates to normalized screen space [-1, 1]
            double u = (2.0 * (x + 0.5) / image_width) - 1.0;
            double v = 1.0 - (2.0 * (y + 0.5) / image_height);

            // ray direction pointing towards scene
            vec3 direction(u, v - 0.4, -1.0);
            ray r(cameraOrigin, direction);

            // sky gradient calculation
            vec3 unitDir = unit_vector(r.direction());
            double t = 0.5 * (unitDir.y() + 2.0);
            color pixelColor = (1.0 - t) * floorColor + t * custom1;

            // floor horizon threshold
            if (v < 0.1) {
                pixelColor = floorColor;
            }

            // ray-sphere intersection checks
            HitRecord rec;
            if (sphereLeft.intersect(r, rec)) {
                pixelColor = rec.shader->rayColor(rec, lightPos, r.direction());
            } else if (sphereMid.intersect(r, rec)) {
                pixelColor = rec.shader->rayColor(rec, lightPos, r.direction());
            } else if (sphereRight.intersect(r, rec)) {
                pixelColor = rec.shader->rayColor(rec, lightPos, r.direction());
            }

            fb.setPixelColor(x, y, pixelColor);
        }
    }

    // export output
    fb.exportToPNG("phong_shading.png");
    std::cout << "Successfully rendered scene to phong_shading.png" << std::endl;

    return 0;
}