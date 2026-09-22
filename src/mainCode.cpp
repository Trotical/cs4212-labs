#include "vec3.h"
#include "framebuffer.h"

#include <iostream>

int main(int argc, char **argv)
{
  Framebuffer fb(200, 200);
  fb.clearToGradient(vec3(0.0, 1.0, 0.0), vec3(1.0, 0.0, 0.0));
  fb.exportToPNG("test_lerp.png");
}