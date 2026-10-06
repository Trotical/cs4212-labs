#pragma once
#include "ray.h" 

class Shape {
public:
    virtual ~Shape() = default;
    
    // pure virtual function that all derived shapes must implement
    virtual bool intersect(const ray& r) const = 0;
};