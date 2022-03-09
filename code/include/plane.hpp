#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// function: ax+by+cz=d
class Plane : public Object3D {
public:
    Plane() {

    }

    Plane(const Vector3f &normal, float d, Material *m) 
    : Object3D(m) 
    , normal(normal)
    , D(-d)
    {
        
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        float t = -(D + Vector3f::dot(normal,r.getOrigin())) / Vector3f::dot(normal, r.getDirection().normalized());
        if (t > h.getT() || t < tmin)
            return false;
        
        Vector3f n = Vector3f::dot(r.getDirection(), normal) > 0 ? -normal : normal;
        h.set(t, n, material->Color, material->type, material->texture_type);
        return true;
    }

protected:
    Vector3f normal;
    float D;

};

#endif //PLANE_H
		

