#include "Framebuffer.h"
#include "png++/png.hpp"
#include <algorithm>

Framebuffer::Framebuffer() : width(100), height(100), fbStorage(width * height) {}

Framebuffer::Framebuffer(int width, int height) : width(width), height(height), fbStorage(width * height) {}

// Clear the framebuffer to a single color.
void Framebuffer::clearToColor(const color &c)
{
  for (size_t idx = 0; idx < fbStorage.size(); idx++) {
    setPixelColor(idx, c);
  }
}

// Clear the framebuffer to a horizontal gradient between two colors.
void Framebuffer::clearToGradient(const color &c1, const color &c2)
{
  for (int x = 0; x < width; x++) {
    double t = (width > 1) ? static_cast<double>(x) / (width - 1) : 0.0;
    color c = (1.0 - t) * c1 + t * c2;

    for (int y = 0; y < height; y++) {
      setPixelColor(x, y, c);
    }
  }
}

// Set pixel color at (i, j)
void Framebuffer::setPixelColor(int i, int j, const color &c)
{
  fbStorage[j * width + i] = c;
}

// Set pixel color at index
void Framebuffer::setPixelColor(int idx, const color &c)
{
  fbStorage[idx] = c;
}

// Export the framebuffer to a PNG file.
void Framebuffer::exportToPNG(const std::string &filename)
{
  png::image<png::rgb_pixel> imData(width, height);

  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      // Uncomment if your coordinate system treats y=0 as the bottom:
      // int row = (height - 1) - j;
      int row = j;

      vec3 color = fbStorage[row * width + i];

      png::byte r = static_cast<png::byte>(256 * std::clamp(color.x(), 0.0, 0.999));
      png::byte g = static_cast<png::byte>(256 * std::clamp(color.y(), 0.0, 0.999));
      png::byte b = static_cast<png::byte>(256 * std::clamp(color.z(), 0.0, 0.999));

      imData[j][i] = png::rgb_pixel(r, g, b);
    }
  }

  imData.write(filename);
}