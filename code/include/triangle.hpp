#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

using namespace std;

class Triangle: public Object3D {

public:
	Triangle() = delete;

    // a b c are three vertex positions of the triangle
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, 
				Material* m, 
				const Vector3f& an = Vector3f::ZERO, 
				const Vector3f& bn = Vector3f::ZERO,
				const Vector3f& cn = Vector3f::ZERO) 
	: Object3D(m) 
	{
		vertices[0] = a; normals[0] = an;
		vertices[1] = b; normals[1] = bn;
		vertices[2] = c; normals[2] = cn;
		if(an == Vector3f::ZERO && bn == Vector3f::ZERO && cn == Vector3f::ZERO)
		{
			normals[0] = (Vector3f::cross(a - b, a - c)).normalized();
			normals[1] = normals[2] = normals[0];
		}
	}

	bool intersect( const Ray& ray,  Hit& hit , float tmin) override {
		Vector3f solve = Matrix3f(ray.getDirection(), 
									vertices[0] - vertices[1], 
									vertices[0] - vertices[2], true).inverse()
						* (vertices[0] - ray.getOrigin()) ;
		float t = solve[0], beta = solve[1], gamma = solve[2];
		if (0 <= beta && 0 <= gamma && (beta + gamma) <= 1)
			if(t < hit.getT() && t > tmin)
			{
				Vector3f ip = interpolate(beta, gamma);
				Vector3f n = Vector3f::dot(ray.getDirection(), ip) > 0 ? -ip : ip;
				hit.set(t, n, getColor(beta, gamma), material->type, material->texture_type);
				return true;
			}
        return false;
	}
	Vector3f normals[3];
	Vector3f vertices[3];
	int texcord[3][2];

	AABB bbox()
	{
		AABB re = AABB({{0,0}, {0,0},{0,0}}, this);
		for(int i = 0; i < 3; ++i)
		{
			re.axis_planes[i][0] = std::min(vertices[0][i], std::min(vertices[1][i], vertices[2][i]));
			re.axis_planes[i][1] = std::max(vertices[0][i], std::max(vertices[1][i], vertices[2][i]));
		}
		return re;
	}

protected:
	

	Vector3f interpolate(float beta, float gamma)
	{
		return normals[0] * (1 - beta - gamma) + normals[1] * beta + normals[2] * gamma;
	}
	inline Vector3f getColor(float beta, float gamma)
	{
		if(material->texture_type == 1)
		{
			float alpha = 1 - beta - gamma;
			int u = texcord[0][0] * alpha + texcord[1][0] * beta + texcord[2][0] * gamma;
			int v = texcord[0][1] * alpha + texcord[1][1] * beta + texcord[2][1] * gamma;
			return material->getUV(u, v);
		}
		return material->Color;
	}
};

#endif //TRIANGLE_H
