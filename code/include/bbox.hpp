#ifndef BBOX_H
#define BBOX_H

#include <vecmath.h>

struct BoundingBox
{
    Vector3f mini;
    Vector3f maxi;

    inline void merge(const Vector3f &value)
    {
        mini.x() = (value.x() < mini.x()) ? value.x() : mini.x();
        mini.y() = (value.y() < mini.y()) ? value.y() : mini.y();
        mini.z() = (value.z() < mini.z()) ? value.z() : mini.z();

        maxi.x() = (value.x() > maxi.x()) ? value.x() : maxi.x();
        maxi.y() = (value.y() > maxi.y()) ? value.y() : maxi.y();
        maxi.z() = (value.z() > maxi.z()) ? value.z() : maxi.z();
    }

    inline void reset()
    {
        mini = Vector3f(D_INF, D_INF, D_INF);
        maxi = Vector3f(-D_INF, -D_INF, -D_INF);
    }
};

#endif