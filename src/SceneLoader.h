#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <iostream>
#include "ISceneLoader.h"
#include "SceneDataContainer.h"
#include "SceneContainer.h"
#include "vec3.h"
#include "shader.h"
#include "sphere.h"
#include "triangle.h"
#include "camera.h"

class SceneLoader : public ISceneLoader {
private:
    SceneContainer &m_targetScene; 
    std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaderMap;

public:
    int numShaders;
    int numTextures;

    SceneLoader(SceneContainer &sceneToPopulate)
        : m_targetScene(sceneToPopulate), numShaders(0), numTextures(0) {}

    void reserveCameras(size_t count) override {
        m_targetScene.cameras.reserve(count);
    }
    
    void reserveLights(size_t count) override {
        m_targetScene.lights.reserve(count);
    }
    
    void reserveShapes(size_t count) override {
        m_targetScene.shapes.reserve(count);
    }
    
    void reserveShaders(size_t count) override { 
        numShaders = count; 
        m_shaderMap.reserve(count);
    }
    
    void reserveTextures(size_t count) override { 
        numTextures = count; 
    }

    void setSceneParams(const SceneParams &sparams) override {
        std::cout << "Setting scene params." << std::endl;
    }

    void addCamera(const std::string &name, const std::string &type,
                const ISceneLoader::vec &pos, const ISceneLoader::vec &viewDir,
                float focalLength, float imagePlaneWidth) override {
    std::cout << "Creating camera: " << name << ", type: " << type << std::endl;
    
    std::shared_ptr<PerspectiveCamera> cam(new PerspectiveCamera());
    cam->type = type;
    
    // Construct glm::vec3 directly from the coordinates
    cam->origin = glm::vec3(pos.x, pos.y, pos.z);
    cam->viewDir = glm::vec3(viewDir.x, viewDir.y, viewDir.z);
    
    cam->focal_length = focalLength;
    cam->plane_width = imagePlaneWidth;
    
    m_targetScene.cameras.push_back(cam);
}

    void addPointLight(const ISceneLoader::vec &pos,
                       const ISceneLoader::vec &intensity) override {
        std::cout << "Creating point light." << std::endl;
        PointLight light;
        light.position = point3(pos.x, pos.y, pos.z);
        light.intensity = color(intensity.x, intensity.y, intensity.z);
        m_targetScene.lights.push_back(light);
    }

    void addAreaLight(const ISceneLoader::vec &pos,
                      const ISceneLoader::vec &color,
                      const ISceneLoader::vec &normal, float width,
                      float length) override {
        std::cout << "Creating area light." << std::endl;
    }

    void addShader(const ISceneLoader::ShaderDesc &shaderDesc) override {
        std::cout << "Creating shader: name=" << shaderDesc.name << ", type=" << shaderDesc.type << std::endl;

        std::shared_ptr<Shader> createdShader = nullptr;

        if (shaderDesc.type == "diffuse" || shaderDesc.type == "lambertian") {
            color diffuseColor(shaderDesc.diffuse.data.x, shaderDesc.diffuse.data.y, shaderDesc.diffuse.data.z);
            createdShader = std::shared_ptr<Shader>(new Lambertian(diffuseColor));
        } 
        else if (shaderDesc.type == "phong" || shaderDesc.type == "blinnphong") {
            color diffuseColor(shaderDesc.diffuse.data.x, shaderDesc.diffuse.data.y, shaderDesc.diffuse.data.z);
            createdShader = std::shared_ptr<Shader>(new BlinnPhong(
                diffuseColor, 
                static_cast<double>(shaderDesc.phongExp)
            ));
        } 
        else if (shaderDesc.type == "mirror" || shaderDesc.type == "reflective") {
            createdShader = std::shared_ptr<Shader>(new MirrorShader());
        } 
        else {
            createdShader = std::shared_ptr<Shader>(new Lambertian(color(0.5, 0.5, 0.5)));
        }

        if (createdShader && !shaderDesc.name.empty()) {
            m_shaderMap[shaderDesc.name] = createdShader;
        }
    }
    void addShape(const ISceneLoader::ShapeDesc &shapeDesc) override {
        std::cout << "Creating shape: type=" << shapeDesc.type << std::endl;

        std::shared_ptr<Shader> assignedShader = nullptr;
        // Use shaderNameReference instead of materialName
        auto it = m_shaderMap.find(shapeDesc.shaderNameReference);
        if (it != m_shaderMap.end()) {
            assignedShader = it->second;
        } else {
            assignedShader = std::make_shared<Lambertian>(VEC3_H(0.7, 0.7, 0.7));
        }

        if (shapeDesc.type == "sphere") {
            point3 center(shapeDesc.center.x, shapeDesc.center.y, shapeDesc.center.z);
            
            // Bypass std::make_shared forwarding quirks using standard new
            std::shared_ptr<Sphere> sphere = std::shared_ptr<Sphere>(
                new Sphere(center, static_cast<double>(shapeDesc.radius), assignedShader)
            );
            
            m_targetScene.shapes.push_back(sphere);
        }
        else if (shapeDesc.type == "triangle") {
            point3 v0(shapeDesc.v0.x, shapeDesc.v0.y, shapeDesc.v0.z);
            point3 v1(shapeDesc.v1.x, shapeDesc.v1.y, shapeDesc.v1.z);
            point3 v2(shapeDesc.v2.x, shapeDesc.v2.y, shapeDesc.v2.z);
            
            std::shared_ptr<Triangle> triangle = std::shared_ptr<Triangle>(
                new Triangle(v0, v1, v2, assignedShader)
            );
            
            m_targetScene.shapes.push_back(triangle);
        }
    }

    void addTexture(const std::string &type, const std::string &name,
                    const std::string &sourceFile) override {
        std::cout << "Creating texture: type=" << type << ", name=" << name << std::endl;
    }
};