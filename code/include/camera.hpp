#ifndef CAMERA_H
#define CAMERA_H

#include "ray.hpp"
#include <vecmath.h>
#include <float.h>
#include <cmath>


class Camera {
public:
    Camera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW, int imgH) {
        this->center = center;
        this->direction = direction.normalized();
        this->horizontal = Vector3f::cross(this->direction, up);
		this->horizontal.normalize();
        this->up = Vector3f::cross(this->horizontal, this->direction);
        this->width = imgW;
        this->height = imgH;
    }

    // Generate rays for each screen-space coordinate
    virtual Ray generateRay(const Vector2f &point) = 0;
    virtual ~Camera() = default;

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    // Extrinsic parameters
    Vector3f center;
    Vector3f direction;
    Vector3f up;
    Vector3f horizontal;

protected:
    
    // Intrinsic parameters
    int width;
    int height;
};

class PerspectiveCamera : public Camera {

public:
    PerspectiveCamera(const Vector3f &center, const Vector3f &direction,
            const Vector3f &up, int imgW, int imgH, float angle) : Camera(center, direction, up, imgW, imgH) {
        // angle is in radian.
        cx = imgW * 0.5;
        cy = imgH * 0.5;
        fx = 2 * znear * tan(angle/2) / imgW;
        fy = 2 * znear * tan(angle/2) / imgH;
        R = Matrix3f(this->horizontal, -(this->up), this->direction);
    }

    Ray generateRay(const Vector2f &point) override {
        //
        Vector3f drc = (Vector3f((point[0] - cx) * fx, (cy - point[1]) * fy, 1));
        return Ray(center, (R*drc).normalized());
    }
private:
    const float znear = 1;
    Matrix3f R;
    float fx, fy;
    float cx, cy;
};

#endif //CAMERA_H
