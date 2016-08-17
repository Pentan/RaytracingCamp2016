#ifndef R1H_INTERSECTION_H
#define R1H_INTERSECTION_H

#include <vector>
#include "r1htypes.h"
#include "ray.h"

namespace r1h {

class Material;
class SceneObject;

class Intersection {
public:
    enum {
        kNoIntersected = -1
    };
    
    R1hFPType distance;
    Vector3 position;
    
    Vector3 hitNormal;
    Vector3 geometryNormal;
    
    Vector3 tangent;
    
    Vector3 varyingWeight;
    int faceId;
    
    int materialId;
    int objectId;
	
    Intersection();
    ~Intersection();
    
    void clear();
};

// extended subclass
class FinalIntersection : public Intersection {
public:
    const Material *material;
    const SceneObject *objectRef;
    
    Vector3 shadingNormal;
    Matrix4 tangentSpaceBasis;
    Matrix4 inverseTangentSpaceBasis;
    
    FinalIntersection(const Intersection& isect);
    ~FinalIntersection();
    
    void computeTangentSpaceWithShadingNormal(const Vector3& shdnorm);
};
    
}

#endif
