#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include <iostream>

// Type alias for color representation using vec3
using color = vec3;

/**
 * Utility function to convert floating-point RGB components from 0 - 1 range
 * to integer 0 - 225
 */
inline void write_color(std::ostream &out, const color &pixel_color)
{
  auto r = pixel_color.x();
  auto g = pixel_color.y();
  auto b = pixel_color.z();

  // Translate the [0,1] component values to the byte range [0,255].
  int rbyte = static_cast<int>(255.999 * r);
  int gbyte = static_cast<int>(255.999 * g);
  int bbyte = static_cast<int>(255.999 * b);

  // Write out the pixel color components
  out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif