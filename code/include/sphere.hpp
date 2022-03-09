#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include "aabb.hpp"

class Sphere : public Object3D {
public:
    Sphere() 
    : center(Vector3f(0))
    , radius(0)
    {
        // unit ball at the center
    }

    Sphere(const Vector3f &center, float radius, Material *material) 
    : Object3D(material) 
    , center(center)
    , radius(radius)
    {
    }

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        Vector3f dir = r.getDirection().normalized();
        Vector3f l = (center - r.getOrigin());      // 球心与视线来源的连线
        float l2 = l.squaredLength();
        bool inside =  (l2 < (radius * radius)-1e-2);       // 光源在球体内部       
        
        float tp = Vector3f::dot(l,dir);   
        if (tp < 0 && !inside)                     // 光线方向与球心方向相反
            return false;
        
        float d2 = (l2 - tp * tp);
        if (d2 > (radius * radius))    // 不相交
            return false;

        float tt = sqrt(radius * radius - d2);
        float t = inside ? (tt + tp) : (tp - tt);
        
        if (h.getT() < t)   // 并非最近的交点
            return false;
        else if (t < tmin)  // 小于最小可能的t值
            return false;
        else
        {
            h.set(t, 
                (r.pointAtParameter(t / r.getDirection().length()) - center).normalized(), 
                getColor(), material->type, material->texture_type);
            return true;
        }

        return false;
    }

    AABB bbox()
    {
        return AABB({{center[0] - radius, center[0] + radius}, {center[1] - radius, center[1] + radius}, {center[2] - radius, center[2] + radius}}, this);
    }

protected:
    float radius;
    Vector3f center;

    Vector3f getColor(const Vector3f& inter = Vector3f::ZERO)
    {
        return material->Color;
    }
};


#endif
