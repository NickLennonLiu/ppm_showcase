#ifndef HITRECORD_H
#define HITRECORD_H

#include <vecmath.h>

struct HitRecord
{
    Vector3f pos;
    Vector3f nrm;
    Vector3f flux;
    Vector3f f;
    double r2;
    unsigned int n;
    int idx;
    bool background;
};

#endif // HITRECORD_H