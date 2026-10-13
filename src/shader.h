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
class Shape; 

color traceRay(
    const ray& r, 
    const std::vector<std::shared_ptr<Shape>>& world, 
    const std::vector<PointLight>& lights, 
    const color& background, 
    int depth
);

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
            if (rec.t > 0.001 && rec.t < lightDistance) {
                return true; 
            }
        }
    }
    return false;
}

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
            if (inShadow(rec.p, light.position, world)) continue;

            vec3 L = unit_vector(light.position - rec.p);
            double nDotL = std::max(0.0, dot(rec.normal, L));

            totalLight += nDotL * albedo * light.intensity;
        }

        return totalLight;
    }
};

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

            totalLight += (diffuse + specular) * light.intensity;
        }

        return totalLight;
    }
};


 
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

        
        vec3 I = unit_vector(viewDir);
        vec3 R = I - 2.0 * dot(I, rec.normal) * rec.normal;
        double directionSign = (dot(I, rec.normal) < 0) ? 1.0 : -1.0;
        ray reflectedRay(rec.p + 0.001 * directionSign * rec.normal, R);

        return traceRay(reflectedRay, world, lights, background, depth - 1);
    }
};
