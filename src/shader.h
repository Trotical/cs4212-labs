#pragma once
#include <memory>
#include <algorithm>
#include <cmath>
#include <vector>
#include "vec3.h"
#include "ray.h"

struct PointLight {
    point3 position;
    color intensity;
};

struct HitRecord;
class Shape; // Forward declaration

// Global ray tracing declaration so MirrorShader can cast secondary reflection rays
color traceRay(
    const ray& r, 
    const std::vector<std::shared_ptr<Shape>>& world, 
    const std::vector<PointLight>& lights, 
    const color& background, 
    int depth
);

// Base Abstract Shader Class
class Shader {
public:
    virtual ~Shader() = default;
    virtual color rayColor(
        const HitRecord& rec, 
        const std::vector<PointLight>& lights, 
        const vec3& viewDir, 
        int depth,
        const std::vector<std::shared_ptr<Shape>>& world,
        const color& background
    ) const = 0;
};

// Helper function to check if a point is in shadow relative to a light source
inline bool inShadow(
    const point3& point, 
    const point3& lightPos, 
    const std::vector<std::shared_ptr<Shape>>& world
) {
    vec3 lightVec = lightPos - point;
    double lightDistance = lightVec.length();
    ray shadowRay(point + 0.001 * unit_vector(lightVec), unit_vector(lightVec));

    HitRecord rec;
    for (const auto& shape : world) {
        if (shape->intersect(shadowRay, rec)) {
            // Shadow hit must occur before reaching the light source position
            if (rec.t > 0.001 && rec.t < lightDistance) {
                return true; // Light is blocked
            }
        }
    }
    return false;
}

// 1. Lambertian Shader with Shadows
class Lambertian : public Shader {
public:
    color albedo;

    Lambertian(const color& a) : albedo(a) {}

    color rayColor(
        const HitRecord& rec, 
        const std::vector<PointLight>& lights, 
        const vec3& viewDir, 
        int depth,
        const std::vector<std::shared_ptr<Shape>>& world,
        const color& background
    ) const override {
        color totalLight(0.0, 0.0, 0.0);

        for (const auto& light : lights) {
            // Skip light contribution if obstructed by an object
            if (inShadow(rec.p, light.position, world)) continue;

            vec3 L = unit_vector(light.position - rec.p);
            double nDotL = std::max(0.0, dot(rec.normal, L));

            // Accumulate diffuse light scaled by light intensity vector
            totalLight += nDotL * albedo * light.intensity;
        }

        return totalLight;
    }
};

// 2. Blinn-Phong Shader with Shadows
class BlinnPhong : public Shader {
public:
    color albedo;
    double phongExp;
    color specularColor;

    BlinnPhong(const color& a, double exp, const color& spec = color(1.0, 1.0, 1.0))
        : albedo(a), phongExp(exp), specularColor(spec) {}

    color rayColor(
        const HitRecord& rec, 
        const std::vector<PointLight>& lights, 
        const vec3& viewDir, 
        int depth,
        const std::vector<std::shared_ptr<Shape>>& world,
        const color& background
    ) const override {
        color totalLight(0.0, 0.0, 0.0);
        vec3 V = unit_vector(-viewDir);

        for (const auto& light : lights) {
            // Skip light contribution if obstructed by an object
            if (inShadow(rec.p, light.position, world)) continue;

            vec3 L = unit_vector(light.position - rec.p);
            double nDotL = std::max(0.0, dot(rec.normal, L));

            color diffuse = nDotL * albedo;
            color specular(0.0, 0.0, 0.0);

            if (nDotL > 0.0 && phongExp > 0.0) {
                vec3 H = unit_vector(L + V);
                double specAngle = std::max(0.0, dot(rec.normal, H));
                specular = std::pow(specAngle, phongExp) * specularColor;
            }

            // Accumulate diffuse + specular scaled by light intensity vector
            totalLight += (diffuse + specular) * light.intensity;
        }

        return totalLight;
    }
};


// 3. Normal Shader
class NormalShader : public Shader {
public:
    color rayColor(
        const HitRecord& rec, 
        const std::vector<PointLight>& lights, 
        const vec3& viewDir, 
        int depth,
        const std::vector<std::shared_ptr<Shape>>& world,
        const color& background
    ) const override {
        return 0.5 * color(rec.normal.x() + 1.0, rec.normal.y() + 1.0, rec.normal.z() + 1.0);
    }
};

// 4. Mirror Shader
// 4. Mirror Shader
class MirrorShader : public Shader {
public:
    color rayColor(
        const HitRecord& rec, 
        const std::vector<PointLight>& lights, 
        const vec3& viewDir, 
        int depth,
        const std::vector<std::shared_ptr<Shape>>& world,
        const color& background
    ) const override {
        if (depth <= 0) return color(0, 0, 0);

        // Incident ray direction normalized (pointing into the surface)
        vec3 I = unit_vector(viewDir);
        
        // Standard reflection formula: R = I - 2 * (I . N) * N
        vec3 R = I - 2.0 * dot(I, rec.normal) * rec.normal;

        // Offset origin along normal to avoid self-intersection (shadow acne)
        // Adjust sign depending on whether ray hits front or back face
        double directionSign = (dot(I, rec.normal) < 0) ? 1.0 : -1.0;
        ray reflectedRay(rec.p + 0.001 * directionSign * rec.normal, R);

        return traceRay(reflectedRay, world, lights, background, depth - 1);
    }
};
