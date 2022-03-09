#ifndef CURVE_HPP
#define CURVE_HPP

#include "object3d.hpp"
#include <vecmath.h>
#include <vector>
#include <utility>

#include <algorithm>

// TODO (PA3): Implement Bernstein class to compute spline basis function.
//       You may refer to the python-script for implementation.
class Bernstein
{
    int n, k;
    std::vector<double> t;
    std::vector<double> tpad;

public:
    // n阶 k次 节点矢量序列t
    Bernstein(int nn, int kk, std::vector<double> knot)
        : n(nn + 1), k(kk), t(knot)
    {
        tpad = t;
        for(int i = 0; i < k; ++i)
            tpad.push_back(t[n+k]);
    }

    // 当n=k, t0=...=tn=0, tn+1=...=t2n+1=1时，是一个次数为n控制顶点数为n+1的Bezier
    static std::vector<double> bezier_knot(int k)
    {
        std::vector<double> re = std::vector<double>(k + 1, 0);
        for(int i = 0; i < k+1; ++i)
            re.push_back(1);
        return re;  
    }

    static std::vector<double> tknots(int n, int k)
    {
        std::vector<double> knots;
        for(int i = 0; i <= n+k+1; ++i)
            knots.push_back(i * 1.0 / (n+k+1));
        return knots;
    }

    // t_bpos <= mu <= t_(bpos+1)
    int get_bpos(double mu)
    {
        int bpos;
        if(!(mu >= t[0]) || !(mu <= t.back()))
            return -1;
        if(mu == t[0])
            bpos = (upper_bound(t.begin(), t.end(), mu) 
                    - t.begin()) - 1;
        else
        {
            bpos = (lower_bound(t.begin(), t.end(), mu) - t.begin()) - 1;
            bpos = std::max(0, bpos);
        }
        return bpos;
    }

    // 合理参数范围的开头
    double vstart()
    {
        return t[k];
    }

    // 合理参数范围的结尾
    double vend()
    {
        // t[-k-1];
        return t[n];
    }

    int evalute(double mu, std::vector<double> &s, std::vector<double> &ds)
    {
        int bpos = get_bpos(mu);
        s = std::vector<double>(k + 2, 0);
        s[k] = 1;

        ds = std::vector<double>(k + 1, 1);
        for(int p = 1; p <= k; ++p)
        {
            for(int ii = k-p; ii <= k; ++ii)
            {
                double w1, dw1, w2, dw2;
                int i = ii + bpos - k;
                if (tpad[i+p] == tpad[i]) {
                    w1 = mu; dw1 = 1;
                } else {
                    w1 = (mu - tpad[i]) / (tpad[i+p] - tpad[i]);
                    dw1 = 1 / (tpad[i+p] - tpad[i]);
                }

                if(tpad[i+p+1] == tpad[i+1]) {
                    w2 = 1 - mu;
                    dw2 = -1;
                } else {
                    w2 = (tpad[i+p+1] - mu) / (tpad[i+p+1] - tpad[i+1]);
                    dw2 = -1 / (tpad[i+p+1] - tpad[i+1]);
                }

                if(p == k)
                    ds[ii] = (dw1 * s[ii] + dw2 * s[ii+1]) * p;
                s[ii] = w1 * s[ii] + w2 * s[ii+1];
            }
        }
        s.pop_back(); // s = s[:-1], 参数曲线的[ti, ti+1]段只受k+1个控制点的控制
        int lsk = bpos - k;
        int rsk = n - bpos - 1;
        if(lsk < 0)
        {
            s = std::vector<double>(s.begin() - lsk, s.end());
            ds = std::vector<double>(ds.begin() - lsk, ds.end());
            lsk = 0;
        }
        if(rsk < 0) {
            s = std::vector<double>(s.begin(), s.end() + rsk);
            ds = std::vector<double>(ds.begin(), ds.end() + rsk);
        }

        return lsk;

    }

};

// The CurvePoint object stores information about a point on a curve
// after it has been tesselated: the vertex (V) and the tangent (T)
// It is the responsiblility of functions that create these objects to fill in all the data.
struct CurvePoint {
    Vector3f V; // Vertex
    Vector3f T; // Tangent  (unit)
};

class Curve : public Object3D {
protected:
    Bernstein *bernstein;
    std::vector<Vector3f> controls;
    std::vector<CurvePoint> samplePoints;
public:
    explicit Curve(std::vector<Vector3f> points) : controls(std::move(points)) {
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        return false;
    }

    std::vector<Vector3f> &getControls() {
        return controls;
    }

    virtual void discretize(int resolution, std::vector<CurvePoint>& data) = 0;

};

class BezierCurve : public Curve {
public:
    explicit BezierCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4 || points.size() % 3 != 1) {
            printf("Number of control points of BezierCurve must be 3n+1!\n");
            exit(0);
        }
        discretize(30, samplePoints); // 在初始化的时候即计算离散点
    }

    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        // resolution: 每一小段[ti, ti_1)中等距采样的点的个数
        int n = controls.size() - 1;
        int k = n;
        std::vector<double> knots = Bernstein::bezier_knot(k);
        bernstein = new Bernstein(n, k, knots);
        double start = bernstein->vstart(), end = bernstein->vend();
        double step = (end - start) / (n+1 - k) / resolution;
        std::vector<double> s, ds;
        while(start < end)
        {
            int lsk = bernstein->evalute(start, s, ds);
            CurvePoint p;
            p.T = p.V = Vector3f();
            for(int i = 0; i < s.size(); ++i)
            {
                p.V += controls[lsk+i] * s[i];
                p.T += controls[lsk+i] * ds[i];
            }
            data.push_back(p);
            start += step;
        }
    }

protected:

};

class BsplineCurve : public Curve {
public:
    BsplineCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4) {
            printf("Number of control points of BspineCurve must be more than 4!\n");
            exit(0);
        }
        discretize(30, samplePoints); // 在初始化的时候即计算离散点
    }

    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        int n = controls.size() - 1, k = 3;
        std::vector<double> knots = Bernstein::tknots(n, k);
        bernstein = new Bernstein(n, k, knots);
        double start = bernstein->vstart(), end = bernstein->vend();
        double step = (end - start) / (n + 1 - k) / resolution;
        std::vector<double> s, ds;
        while (start < end)
        {
            int lsk = bernstein->evalute(start, s, ds);
            CurvePoint p;
            p.T = p.V = Vector3f();
            for (int i = 0; i < s.size(); ++i)
            {
                p.V += controls[lsk + i] * s[i];
                p.T += controls[lsk + i] * ds[i];
            }
            data.push_back(p);
            start += step;
        }
    }

protected:

};

#endif // CURVE_HPP
