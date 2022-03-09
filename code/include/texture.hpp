#include "FractalNoise.hpp"
#include "Perlin.hpp"
#include "material.hpp"
#include <cmath>

float cut(float x){
    return x - (int)x;
}

//https://www.researchgate.net/publication/340769728_REALISTIC_RENDERING_OF_WOOD
Vector3f wood(const Vector3f& x, Perlin* gen)
{
    float g = fabs(sin(x.x() + 2 * gen->noise(x.x(), x.y(), x.z()))) * 10;
    return cut(g) * Vector3f(0.345, 0.188, 0.074);
}

Vector3f marble(const Vector3f& x, Perlin* gen)
{
    return (cos((3 * x.x() -  2 * x.y() + 20 * gen->noise(x.x(), x.y(), x.z()))) + 1)/2 * Vector3f(0.761, 0.698, 0.502);
}

Vector3f rainbow(const Vector3f& x, Perlin* gen)
{
    float z = abs(x.z()*5);
    float zz = z - floor(z);
    switch((int)z % 6)
    {
        case 0: return Vector3f(1,0, 1 - zz) * 0.8;
        case 1: return Vector3f(1,zz, 0) * 0.8 ;
        case 2: return Vector3f(1-zz, 1, 0) * 0.8;
        case 3: return Vector3f(0, 1, zz) * 0.8;
        case 4: return Vector3f(0, 1 - zz, 1) * 0.8;
        case 5: return Vector3f(zz, 0, 1) * 0.8;
        default: return Vector3f(0,0,0) * 0.8;
    }
}