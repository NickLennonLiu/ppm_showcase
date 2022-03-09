#ifndef LIGHT_H
#define LIGHT_H

#include <Vector3f.h>
#include "object3d.hpp"
#include "halton.hpp"
#include "ray.hpp"

class Light {
public:
    Light() = default;

    virtual ~Light() = default;

    virtual void genp(Ray &pr, Vector3f *f, int i) = 0;
    
    Vector3f color;
    Vector3f flux;

};


class DirectionalLight : public Light {
    Vector3f direction;
    Vector3f position;
    Vector3f axis1, axis2;
public:
    DirectionalLight() = delete;

    DirectionalLight(const Vector3f &d, const Vector3f &c, 
    const Vector3f &p, const Vector3f &a1, const Vector3f &a2, const Vector3f& f = Vector3f({100, 100, 100}))
    : position(p)
    , axis1(a1)
    , axis2(a2)
    {
        flux = f;
        direction = d.normalized();
        color = c;
    }

    ~DirectionalLight() override = default;

    virtual void genp(Ray &pr, Vector3f *f, int i) override
    {
        (*f) = flux * (D_PI * 4.0) * color; // flux
        auto x1 = axis1 * halton(0, i),
             x2 = axis2 * halton(1, i);
        pr = Ray(position + x1 + x2, direction);
    }
};

class AreaLight: public Light {
    Vector3f position;
    Vector3f axis1, axis2;
    Vector3f n1, n2;
    Vector3f direction;
public:
    AreaLight() = delete;

    AreaLight(const Vector3f &c, const Vector3f &p, 
            const Vector3f &a1, const Vector3f &a2, 
            const Vector3f& f = Vector3f({100, 100, 100}))
    : position(p)
    , axis1(a1), axis2(a2)
    {
        flux = f;
        color = c;
        n1 = axis1.normalized(), n2 = axis2.normalized();
        direction = Vector3f::cross(a1, a2).normalized();
    }

    ~AreaLight() override = default;

    virtual void genp(Ray& pr, Vector3f *f, int i) override
    {
        (*f) = flux * (D_PI * 4.0) * color; // flux
        auto angle = 2.0 * D_PI * halton(0, i);
        auto radius = halton(3,i);
        auto x1 = axis1 * sin(angle),
             x2 = axis2 * cos(angle);
        auto p = 2.0 * D_PI * halton(0, i);
        auto t = 2.0 * acos(sqrt(1. - halton(1, i)));
        auto st = sin(t);
        pr = Ray(position + (x1 + x2) * radius, Vector3f(cos(p) * st, cos(t), sin(p) * st));
        /*
        auto p = 2.0 * D_PI * halton(1, i);
        auto t = halton(2, i);
        t = 1 - t*t;
        pr = Ray(position + (x1 + x2) * radius, direction * t + (n1 * sin(p) + n2 * cos(p)) * sqrt(1 - t * t));
        */
    }
};

class PointLight : public Light {
    Vector3f position;
public:
    PointLight() = delete;

    PointLight(const Vector3f &p, const Vector3f &c, const Vector3f& f = Vector3f({100, 100, 100})) {
        position = p;
        color = c;
        flux = f;
    }

    ~PointLight() override = default;

    virtual void genp(Ray &pr, Vector3f *f, int i) override
    {
        (*f) = flux * (D_PI * 4.0) * color; // flux
        auto p = 2.0 * D_PI * halton(0, i);
        auto t = 2.0 * acos(sqrt(1. - halton(1, i)));
        auto st = sin(t);
        pr = Ray(position, Vector3f(cos(p) * st, cos(t), sin(p) * st));
    }
};

#endif // LIGHT_H
