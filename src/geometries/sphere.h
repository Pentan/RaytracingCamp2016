#ifndef R1H_SPHERE_H
#define R1H_SPHERE_H

#include "geometry.h"

namespace r1h {

class Sphere : public Geometry {
    
public:
    //static const int kTypeID = 'SHPR';
    
    R1hFPType radius;
    Vector3 position;
    
	Sphere();
	Sphere(const R1hFPType r, const Vector3 &pos);
    ~Sphere();
    
    AABB getAABB(const Matrix4& tm) const;
	bool isIntersect(const Ray &ray, Intersection *intersect) const;
    SamplePoint getSamplePoint(Random *rng) const;
};

}
#endif
