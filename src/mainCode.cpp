#include "vec3.h"
#include "framebuffer.h"
#include "ray.h"
#include "sphere.h"

#include <iostream>

int main(int argc, char **argv)
{
    int width = 400;  // Adjusted width for flag ratio
    int height = 267; // Adjusted height for 3:2 aspect ratio

    Framebuffer fb(width, height);

    //  define colors
    vec3 red(1.0, 0.0, 0.0);
    vec3 white(1.0, 1.0, 1.0);

    //  define the sphere in scene space
    Sphere sphere(vec3(0.0, 0.0, -5.0), 2.0);

    //  define camera origin
    vec3 cameraOrigin(0.0f, 0.0f, 0.0f);

    // 4. loop through each pixel in the framebuffer
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            
            // map pixel coordinates (x, y) to normalized screen coordinates [-1, 1]
            float u = (2.0f * (x + 0.5f) / width) - 1.0f;
            float v = (2.0f * (y + 0.5f) / height) - 1.0f;
            
            // correct aspect ratio so the sphere ain't stretched
            float aspectRatio = static_cast<float>(width) / height;
            u *= aspectRatio;

            // direction vector pointing into the scene along -Z
            vec3 rayDirection = vec3(u, v, -1.0f);

            // construct ray from camera through current pixel
            ray r(cameraOrigin, rayDirection);

            // set color based on ray-sphere intersection
            if (sphere.intersect(r)) {
                fb.setPixelColor(x, y, red);   // sphere hit -> red
            } else {
                fb.setPixelColor(x, y, white); // miss -> white background
            }
        }
    }

    // export the generated flag
    fb.exportToPNG("japan_flag.png");
    std::cout << "Successfully generated japan_flag.png!" << std::endl;

    return 0;
}