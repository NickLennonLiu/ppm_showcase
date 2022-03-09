#include "mesh.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <sstream>

bool Mesh::intersect(const Ray &r, Hit &h, float tmin) {

    // Optional: Change this brute force method into a faster one.
    bool re = root->intersect(r, h, tmin);
    return re;
}

Mesh::Mesh(const char *filename, Material *material) : Object3D(material) {
    // Optional: Use tiny obj loader to replace this simple one.
    std::ifstream f;
    f.open(filename);
    if (!f.is_open()) {
        std::cout << "Cannot open " << filename << "\n";
        return;
    }
    std::string line;
    std::string vTok("v");
    std::string fTok("f");
    std::string texTok("vt");
    char bslash = '/', space = ' ';
    std::string tok;
    int texID;
    while (true) {
        std::getline(f, line);
        if (f.eof()) {
            break;
        }
        if (line.size() < 3) {
            continue;
        }
        if (line.at(0) == '#') {
            continue;
        }
        std::stringstream ss(line);
        ss >> tok;
        if (tok == vTok) {  // Vertices
            Vector3f vec;
            ss >> vec[0] >> vec[1] >> vec[2];
            v.push_back(vec);
            vts.push_back({0,0});
        } else if (tok == fTok) {   // Faces
            if (line.find(bslash) != std::string::npos) {
                std::replace(line.begin(), line.end(), bslash, space);
                std::stringstream facess(line);
                TriangleIndex trig;
                facess >> tok;
                for (int ii = 0; ii < 3; ii++) {
                    facess >> trig[ii] >> texID;
                    trig[ii]--;
                }
                t.push_back(trig);
            } else {
                TriangleIndex trig;
                for (int ii = 0; ii < 3; ii++) {
                    ss >> trig[ii];
                    trig[ii]--;
                }
                t.push_back(trig);
            }
        } else if (tok == texTok) { // Texture point
            int vid, u, v;
            ss >> vid >> u >> v;
            vts[vid-1].first = u;
            vts[vid-1].second = v;
        }
    }
    computeNormal();
    getKdTree();
    f.close();
}

void Mesh::computeNormal() {
    n.resize(t.size());
    for (int triId = 0; triId < (int) t.size(); ++triId) {
        TriangleIndex& triIndex = t[triId];
        Vector3f a = v[triIndex[1]] - v[triIndex[0]];
        Vector3f b = v[triIndex[2]] - v[triIndex[0]];
        b = Vector3f::cross(a, b);
        n[triId] = b / b.length();
    }
}

void Mesh::getKdTree() {
    vector<AABB> bboxs;
    for (int triId = 0; triId < (int)t.size(); ++triId)
    {
        TriangleIndex &triIndex = t[triId];
        Triangle triangle(v[triIndex[0]],
                          v[triIndex[1]], v[triIndex[2]], material);

        //TODO: Figure out a way to specify vertice norm
        triangle.normals[0] = n[triId];
        triangle.normals[1] = n[triId];
        triangle.normals[2] = n[triId];
        triangle.texcord[0][0] = vts[triIndex[0]].first;
        triangle.texcord[0][1] = vts[triIndex[0]].second;
        triangle.texcord[1][0] = vts[triIndex[1]].first;
        triangle.texcord[1][1] = vts[triIndex[1]].second;
        triangle.texcord[2][0] = vts[triIndex[2]].first;
        triangle.texcord[2][1] = vts[triIndex[2]].second;
        triangles.push_back(triangle);
        //bboxs.push_back(triangle.bbox());
    }
    for(auto &i : triangles)
    {
        AABB tbox = i.bbox();
        box = AABB::merge(box, tbox);
        bboxs.push_back(tbox);
    }
    root = KdNode::split(0, bboxs.size(), 0, bboxs);
    box.content = this;
}

AABB Mesh::bbox()
{
    return box;
}
