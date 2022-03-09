#ifndef HIT_H
#define HIT_H

#include <vecmath.h>
#include "ray.hpp"

class Material;

class Hit {
public:

    // constructors
    Hit() {
        f = Vector3f();
        type = 0;
        t = 1e38;
    }

    Hit(float _t, const Vector3f &n, const Vector3f &_f = Vector3f::ZERO, int _type = 0, int _ttype = 0) {
        t = _t;
        normal = n;
        f = _f;
        type = _type;
        ttype = _ttype;
    }

    Hit(const Hit &h) {
        t = h.t;
        normal = h.normal;
        f = h.f;
        type = h.type;
        ttype = h.ttype;
    }

    // destructor
    ~Hit() = default;

    float getT() const {
        return t;
    }

    const Vector3f &getNormal() const {
        return normal;
    }

    void set(float _t, const Vector3f &n, const  Vector3f& _f = Vector3f::ZERO, int _type = 0, int _ttype = 0) {
        t = _t;
        f = _f;
        type = _type;
        ttype = _ttype;
        normal = n;
    }

    float t;
    Vector3f f;
    int type, ttype;
    Vector3f normal;

};

inline std::ostream &operator<<(std::ostream &os, const Hit &h) {
    os << "Hit <" << h.getT() << ", " << h.getNormal() << ">";
    return os;
}

#endif // HIT_H
