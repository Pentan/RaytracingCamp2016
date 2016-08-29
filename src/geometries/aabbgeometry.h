#ifndef R1H_AABBGEOMETRY_H
#define R1H_AABBGEOMETRY_H

#include "geometry.h"

namespace r1h {

class AABBGeometry : public Geometry {
public:
    static const int kTypeID;
    
    AABB aabb;
    
    AABBGeometry(const Vector3 min, const Vector3 max);
    AABBGeometry(const AABB inaabb);
    AABB getAABB(const Matrix4& tm) const;
    
    bool isIntersect(const Ray &ray, Intersection *intersect) const;
    SamplePoint getSamplePoint(Random *rng) const;
};

}
#endif
