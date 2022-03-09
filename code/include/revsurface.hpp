#ifndef REVSURFACE_HPP
#define REVSURFACE_HPP

#include "object3d.hpp"
#include "curve.hpp"
#include "kdnode.hpp"
#include "aabb.hpp"
#include "triangle.hpp"
#include <tuple>

class RevSurface : public Object3D {

    Curve *pCurve;
    KdNode* root;
    std::vector<Triangle> triangles;
    AABB box;
public:
    RevSurface(Curve *pCurve, Material* material) : pCurve(pCurve), Object3D(material) {
        // Check flat.
        // 旋转曲面所使用的样条曲线必须位于x-y平面上。
        for (const auto &cp : pCurve->getControls()) {
            if (cp.z() != 0.0) {
                printf("Profile of revSurface must be flat on xy plane.\n");
                exit(0);
            }
        }
        // Definition for drawable surface.
        // 由三个顶点及其法向量决定的面
        
        // Surface is just a struct that contains vertices, normals, and
        // faces.  VV[i] is the position of vertex i, and VN[i] is the normal
        // of vertex i.  A face is a triple i,j,k corresponding to a triangle
        // with (vertex i, normal i), (vertex j, normal j), ...
        // Currently this struct is computed every time when canvas refreshes.
        // You can store this as member function to accelerate rendering.

        std::vector<CurvePoint> curvePoints;
        pCurve->discretize(30, curvePoints); // generate curve with resolution of 30, store it into curvePoints
        const int steps = 40;
        triangles.clear();
        int csize = curvePoints.size();
        // For all points on the curve
        for (unsigned int ci = 0; ci < curvePoints.size(); ++ci) {
            const CurvePoint &cp = curvePoints[ci];
            for (unsigned int i = 0; i < steps; ++i) {
                float t = (float) i / steps;
                Quat4f rot;
                rot.setAxisAngle(t * 2 * 3.14159, Vector3f::UP);
                // rotated point
                Vector3f pnew = Matrix3f::rotation(rot) * cp.V;
                // original norm
                Vector3f pNormal = Vector3f::cross(cp.T, -Vector3f::FORWARD);
                // rotated norm
                Vector3f nnew = Matrix3f::rotation(rot) * pNormal;
                
                surface.VV.push_back(pnew);
                surface.VN.push_back(nnew);
                
                // circling next
                int i1 = (i + 1 == steps) ? 0 : i + 1;
                if (ci != curvePoints.size() - 1) {
                    // emplace_back: construct new element inplace.
                    surface.VF.emplace_back((ci + 1) * steps + i, ci * steps + i1, ci * steps + i, 
                                            std::make_pair((i * 500) / steps, ((ci+1) * 500) / csize), 
                                            std::make_pair((i+1) * 500 / steps, ci * 500 / csize), 
                                            std::make_pair((i * 500 / steps), ci * 500 / csize));
                    surface.VF.emplace_back((ci + 1) * steps + i, (ci + 1) * steps + i1, ci * steps + i1, 
                                            std::make_pair((i * 500) / steps, ((ci+1) * 500) / csize), 
                                            std::make_pair((i+1) * 500 / steps, (ci+1) * 500 / csize), 
                                            std::make_pair((i+1) * 500 / steps, ci * 500 / csize));
                }
            }
        }
        
        for (unsigned i = 0; i < surface.VF.size(); i++) {
            Triangle face(  surface.VV[std::get<0>(surface.VF[i])],
                            surface.VV[std::get<1>(surface.VF[i])],
                            surface.VV[std::get<2>(surface.VF[i])],
                            material,
                            surface.VN[std::get<0>(surface.VF[i])],
                            surface.VN[std::get<1>(surface.VF[i])],
                            surface.VN[std::get<2>(surface.VF[i])]);
            face.texcord[0][0] = std::get<3>(surface.VF[i]).first;
            face.texcord[0][1] = std::get<3>(surface.VF[i]).second;
            face.texcord[1][0] = std::get<4>(surface.VF[i]).first;
            face.texcord[1][1] = std::get<4>(surface.VF[i]).second;
            face.texcord[2][0] = std::get<5>(surface.VF[i]).first;
            face.texcord[2][1] = std::get<5>(surface.VF[i]).second;

            triangles.push_back(face);
        }
        
        std::vector<AABB> bboxs;
        for(auto &i : triangles)
        {
            AABB tbox = i.bbox();
            box = AABB::merge(box, tbox);
            bboxs.push_back(tbox);
        }
        root = KdNode::split(0, bboxs.size(), 0, bboxs);
        box.content = this;
    }

    AABB bbox() override
    {
        return box;
    }

    ~RevSurface() override {
        delete pCurve;
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        return root->intersect(r, h, tmin);
    }

    typedef std::tuple<unsigned, unsigned, unsigned, std::pair<int, int>, std::pair<int, int>, std::pair<int, int>> Tup;
    // 表面所存储的顶点、顶点对应的法向量、
    struct Surface {
        std::vector<Vector3f> VV;
        std::vector<Vector3f> VN;
        std::vector<Tup> VF;
    } surface;
    
};

#endif //REVSURFACE_HPP
