#pragma once
#include <memory>
#include <algorithm>
#include <cmath>
#include "vec3.h"
#include "ray.h"
#include "shape.h"

// Forward declaration of HitRecord
struct HitRecord;

// Base Abstract Shader Class
class Shader {
public:
    virtual ~Shader() = default;
    virtual color rayColor(const HitRecord& rec, const point3& lightPos, const vec3& viewDir) const = 0;
};

// 1. Lambertian (Diffuse) Shader
class Lambertian : public Shader {
public:
    color albedo;

    Lambertian(const color& a) : albedo(a) {}

    color rayColor(const HitRecord& rec, const point3& lightPos, const vec3& viewDir) const override {
        vec3 L = unit_vector(lightPos - rec.p);
        
        // Lambertian cosine law: max(0, N . L)
        double nDotL = std::max(0.0, dot(rec.normal, L));
        return nDotL * albedo;
    }
};

// 2. Blinn-Phong Shader (Diffuse + Specular)
class BlinnPhong : public Shader {
public:
    color albedo;
    double phongExp;
    color specularColor;

    BlinnPhong(const color& a, double exp, const color& spec = color(1.0, 1.0, 1.0))
        : albedo(a), phongExp(exp), specularColor(spec) {}

    color rayColor(const HitRecord& rec, const point3& lightPos, const vec3& viewDir) const override {
        vec3 L = unit_vector(lightPos - rec.p);
        vec3 V = unit_vector(-viewDir); // View vector towards camera

        // 1. Diffuse component
        double nDotL = std::max(0.0, dot(rec.normal, L));
        color diffuse = nDotL * albedo;

        // 2. Specular component using Half-Vector H
        color specular(0.0, 0.0, 0.0);
        if (nDotL > 0.0 && phongExp > 0.0) {
            vec3 H = unit_vector(L + V); // Half-vector
            double specAngle = std::max(0.0, dot(rec.normal, H));
            specular = std::pow(specAngle, phongExp) * specularColor;
        }

        return diffuse + specular;
    }
};