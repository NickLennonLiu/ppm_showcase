#ifndef AABB_HPP
#define AABB_HPP

#include <vecmath.h>
#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>
using namespace std;

const float dir[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

class Object3D;

class AABB
{
    //static float dir[3][3];
    // 3个平面确定一个包围盒
    
public:
    float u, v;
    float axis_planes[3][2]; // 0:x, 1:y, 2:z // 0: min, 1: max
    AABB() {}
    explicit AABB(std::vector<std::pair<float, float>> axis, Object3D* content = nullptr)
    : content(content)
    {
        axis_planes[0][0] = axis[0].first, axis_planes[0][1] = axis[0].second;
        axis_planes[1][0] = axis[1].first, axis_planes[1][1] = axis[1].second;
        axis_planes[2][0] = axis[2].first, axis_planes[2][1] = axis[2].second;
    }
    bool intersect(const Ray &r, Hit &h, float tmin)
    {
        Vector3f o = r.getOrigin();
        if(inside(o))
            return true;
        float tm = 1e38;
        int idx = 0;
        bool re = false;
        Hit ans;
        for(int i = 0; i < 3; ++i)
        {
            int close = (fabsf(axis_planes[i][0] - o[i]) > fabsf(axis_planes[i][1] - o[i])) ? 1 : 0;
            Hit ht;
            float temp;
            if (get_axis_plane_t(i, close, r, ht))
            {
                temp = ht.getT();
            }
            else 
                continue;
            Vector3f inte = r.pointAtParameter(ht.getT());
            bool inside = true;
            for (int j = 0; j < 3; ++j)
            {
                if (j == i)
                    continue;
                if ((inte[j] > axis_planes[j][1]) || (inte[j] < axis_planes[j][0]))
                {
                        
                    inside = false;
                    break;
                }
            }

            if((temp < tm) && inside)
            {
                tm = temp;
                idx = i;
                ans = ht;
                re = true;
            }
        }
        
        if(ans.getT() > h.getT() || ans.getT() < tmin)
            return false;
        h = ans;
        return re;
    }

    bool inside(const Vector3f& v)
    {
        if(v.x() < axis_planes[0][0] || v.x() > axis_planes[0][1])
            return false;
        if (v.y() < axis_planes[1][0] || v.y() > axis_planes[1][1])
            return false;
        if (v.z() < axis_planes[2][0] || v.z() > axis_planes[2][1])
            return false;
        return true;
    }

    void debug()
    {
        for(int i = 0; i < 3; ++i)
        {
            for(int j = 0; j < 2; ++j)
                cout << axis_planes[i][j] << " ";
        }
        cout << content;
        cout << endl;
    }

    Object3D* content;

    static AABB merge(const AABB &a, const AABB &b)
    {
        AABB re;
        re.content = nullptr;
        for (int i = 0; i < 3; ++i)
        {
            re.axis_planes[i][0] = min(a.axis_planes[i][0], b.axis_planes[i][0]);
            re.axis_planes[i][1] = max(a.axis_planes[i][1], b.axis_planes[i][1]);
        }
        return re;
    }



protected:
    bool get_axis_plane_t(int axis, int direction, const Ray& r, Hit &h)
    {
        Vector3f normal(dir[axis][0], dir[axis][1], dir[axis][2]);
        float D = -axis_planes[axis][direction];
        float t = -(D + Vector3f::dot(normal, r.getOrigin())) / Vector3f::dot(normal, r.getDirection());
        if (t > h.getT() || t < 0)
            return false;

        Vector3f n = Vector3f::dot(r.getDirection(), normal) > 0 ? -normal : normal;
        h.set(t, n);
        return true;
    }
};

inline std::ostream &operator<<(std::ostream &os, const AABB &a)
{
    os << a.axis_planes[0][0] << " " << a.axis_planes[0][1] << " "
       << a.axis_planes[1][0] << " " << a.axis_planes[1][1] << " "
       << a.axis_planes[2][0] << " " << a.axis_planes[2][1] << " ";
    return os;
}

#endif // AABB_HPP