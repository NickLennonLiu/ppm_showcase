#include "curve.hpp"
#include <vector>
#include "object3d.hpp"
#include "kdnode.hpp"
#include "triangle.hpp"

struct SurfacePoint {
    Vector3f V;
    Vector3f uT, vT;
    Vector3f N;
};

class Surface : public Object3D
{
protected:
    int m, n; // u - m, v - n
    std::vector<std::vector<Vector3f>> controls;
    std::vector<std::vector<SurfacePoint>> samplePoints;
    KdNode* root;
    std::vector<Triangle> triangles;
    AABB box;
    Bernstein *ub, *vb;
    typedef std::tuple<unsigned, unsigned, unsigned> Tup3u;
    // 表面所存储的顶点、顶点对应的法向量、
    struct Face {
        std::vector<Vector3f> VV;
        std::vector<Vector3f> VN;
        std::vector<Tup3u> VF;
    } faces;


public:
    explicit Surface(std::vector<std::vector<Vector3f>> points, Material* material) 
    : Object3D(material)
    , controls(std::move(points)) {
        m = controls.size();
        n = controls[0].size();
    }

    virtual bool intersect(const Ray&r, Hit &h, float tmin) override {
        return false;
    }

    virtual void discretize(int resolution, std::vector<std::vector<SurfacePoint>>& data) = 0;


};

class BezierSurface : public Surface {
public:
    explicit BezierSurface(const std::vector<std::vector<Vector3f>> &points, Material* material) : Surface(points, material) {
        if ((n < 4 || n % 3 != 1) || (m < 4 || m % 3 != 1)) {
            printf("Number of control points of axis of BezierSurface must be 3n+1!\n");
            exit(0);
        }
        discretize(30, samplePoints); // 在初始化的时候即计算离散点
        Meshify();
    }

    void Meshify()
    {
        int row = samplePoints.size();
        int col = samplePoints[0].size();
        triangles.clear();
        std::vector<AABB> bboxs;
        for(int i = 0; i < row-1; ++i)
        {
            for(int j = 0; j < col-1; ++j)
            {
                Triangle face1(samplePoints[i][j].V, 
                                samplePoints[i+1][j].V,
                                samplePoints[i][j+1].V,
                                material, 
                                samplePoints[i][j].N,
                                samplePoints[i+1][j].N,
                                samplePoints[i][j+1].N);
                
                Triangle face2(samplePoints[i+1][j+1].V, 
                                samplePoints[i+1][j].V,
                                samplePoints[i][j+1].V,
                                material, 
                                samplePoints[i+1][j+1].N,
                                samplePoints[i+1][j].N,
                                samplePoints[i][j+1].N);
                
                triangles.push_back(face1);
                triangles.push_back(face2);
            }
        }
        for(int i = 0; i < row-1; ++i)
        {
            for(int j = 0; j < col - 1; ++j)
            {
                AABB box1 = triangles[(i * (row-1) + j) << 1].bbox();
                box1.u = (float) i / row, box1.v = (float) j / col;
                bboxs.push_back(box1);
                AABB box2 = triangles[((i * (row-1) + j) << 1) + 1].bbox();
                box2.u = (float) (i+1) / row, box2.v = (float) (j+1) / col;
                bboxs.push_back(box2);
                box = AABB::merge(AABB::merge(box1, box2), box);
            }
        }
        root = KdNode::split(0, bboxs.size(), 0, bboxs);
        box.content = this;
    }

    Vector3f SingleNewton(const Ray& ray, Vector3f tuv, float& dis, 
                    std::vector<double>& us, std::vector<double>& uds, 
                    std::vector<double>& vs, std::vector<double>& vds, 
                    Vector3f& un, Vector3f &vn) const
    {
        //F'(x) = [ray.direction, - delta P/delta u, - delta P/delta v]
        us.clear(), uds.clear(), vs.clear(), vds.clear();
        int ulsk = ub->evalute(tuv.y(), us, uds);
        int vlsk = vb->evalute(tuv.z(), vs, vds);
        Vector3f p = ray.pointAtParameter(tuv.x()); // F(xi)
        Vector3f ud;    // delta P/ delta u
        Vector3f vd;    // delta P/ delta v
        for(int i = 0; i < us.size(); ++i)
        {
            for(int j = 0; j < vs.size(); ++j)
            {
                Vector3f point = controls[ulsk+i][vlsk+j];
                p -= point * us[i] * vs[j];
                ud += point * uds[i] * vs[j];
                vd += point * us[i] * vds[j];
            }
        }
        dis = p.length();
        Matrix3f Fi = Matrix3f(ray.direction, -ud, -vd).inverse();
        un = ud, vn = vd;
        return tuv - Fi * p;  // x_{i+1}
    }

    bool Newton(const Ray& ray, float t, float u, float v, float& ret, Vector3f& un, Vector3f& vn)
    {
        float dis;
        vector<double> us, uds, vs, vds;
        Vector3f next = SingleNewton(ray, Vector3f(t, u, v), dis, us, uds, vs, vds, un, vn);
        int step = 0;
        while(dis > 0.1 && step < 10)
        {
            next = SingleNewton(ray, next, dis, us, uds, vs, vds, un, vn);
            step++;
        }
        if(step == 10)  // stop early
            return false;
        ret = next.x();
        return true;
    }

    void discretize(int resolution, std::vector<std::vector<SurfacePoint>> & data) override {
        data.clear();
        std::vector<double> uknots = Bernstein::bezier_knot(m-1),
                            vknots = Bernstein::bezier_knot(n-1);
        ub = new Bernstein(m-1,m-1,uknots);
        vb = new Bernstein(n-1,n-1,vknots);
        double ustart = ub->vstart(), uend = ub->vend(),
            vstart = vb->vstart(), vend = vb->vend();
        double ustep = (uend - ustart) / resolution,
                vstep = (vend - vstart) / resolution;
        std::vector<double> us, uds, vs, vds;
        us.clear();
        uds.clear();
        vs.clear();
        vds.clear();
        for(float u = ustart; u < uend; u += ustep)
        {
            std::vector<SurfacePoint> row;
            int ulsk = ub->evalute(u, us, uds);
            for(float v = vstart; v < vend; v+= vstep)
            {
                int vlsk = vb->evalute(v, vs, vds);                   
                SurfacePoint p;
                for(int i = 0; i < us.size(); ++i)
                    for(int j = 0; j < vs.size(); ++j)
                    {
                        Vector3f point = controls[ulsk+i][vlsk+j];
                        p.V += point * us[i] * vs[j];
                        p.uT += point * uds[i] * vs[j];
                        p.vT += point * us[i] * vds[j]; 
                        p.N = Vector3f::cross(p.uT, p.vT).normalized();
                    }
                row.push_back(p);   
            }
            data.push_back(row);
        }
    }

    AABB bbox() override
    {
        return box;
    }

    ~BezierSurface() override {

    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        //return root->intersect(r, h, tmin); // Use Mesh to intersect
        std::queue<KdNode*> que;
        que.push(root);
        Hit tmp;
        Hit th;
        float result;
        Vector3f un, vn;
        bool re = false;
        while(!que.empty())
        {
            th = tmp;
            KdNode* cur = que.front();
            que.pop();
            if(!cur->bbox.intersect(r, th, tmin))
                continue;
            
            if(cur->lc == nullptr && cur->rc == nullptr)
            {
                float u = cur->bbox.u, v = cur->bbox.v;
                Newton(r, th.t, u, v, result, un, vn);
                if(result > tmin && result < h.t)
                {
                    re = true;
                    h.set(result, Vector3f::cross(un, vn).normalized(), material->Color, material->type, material->texture_type);
                }
                    
            }
            if(cur->lc)
                que.push(cur->lc);
            if(cur->rc)
                que.push(cur->rc);       
        }
        return re;
    }
};