#ifndef GROUP_H
#define GROUP_H


#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <vector>
#include "kdnode.hpp"
#include <algorithm>
#include <iostream>


class Group : public Object3D {

public:

    Group() {

    }

    explicit Group (int num_objects) {
        this->num_objects = num_objects;
    }

    ~Group() override {    
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        return root->intersect(r, h, tmin);
    }

    void addObject(int index, Object3D *obj) {
        objs.insert(objs.begin() + index, obj);
        bboxs.push_back(obj->bbox());
    }

    int getGroupSize() {
        return num_objects;
    }

    void getKdTree()
    {
        root = KdNode::split(0, num_objects, 0, bboxs);
    }

private:
    std::vector<Object3D*> objs;    
    std::vector<AABB> bboxs;
    int num_objects;
    KdNode* root;
};

#endif
	
