
#include "intersection.h"

using namespace r1h;

Intersection::Intersection():
	objectId(kNoIntersected),
    distance(kINF),
    position(),
	hitNormal(),
    geometryNormal(),
    tangent(0.0)
{}

Intersection::~Intersection() {
}

/*
Vector3 Intersection::orientingNormal(const Ray &ray) const {
	return (Vector3::dot(normal, ray.direction) < 0.0)? normal : (normal * -1.0);
}
*/
void Intersection::clear() {
    distance = kINF;
    objectId = -1;
}

/////////

FinalIntersection::FinalIntersection(const Intersection& isect) :
    Intersection(isect)
{
}

FinalIntersection::~FinalIntersection() {
}

void FinalIntersection::computeTangentSpaceWithShadingNormal(const Vector3& shdnorm) {
    shadingNormal = shdnorm;
    
    if(tangent.isZero(kEPS)) {
        // tangent is not defined. generate it from shadingNormal
        tangent = Vector3::cross(Vector3(0.0, 0.0, 1.0), shadingNormal);
        if(!tangent.normalize()) {
            tangent.set(1.0, 0.0, 0.0);
        }
    }
    
    Vector3 basisx = tangent;
    Vector3 basisy;
    Vector3 basisz = shadingNormal;
    
    basisy = Vector3::cross(basisz, basisx);
    basisy.normalize();
    
    tangentSpaceBasis.setBasis(basisx, basisy, basisz);
    inverseTangentSpaceBasis = Matrix4::transposed(tangentSpaceBasis);
}

/*
Vector3 FinalIntersection::smallOffsetedPosition(bool into) const {
    if(into) {
        return position + shadingNormal * kSmallOffset;
    } else {
        return position - shadingNormal * kSmallOffset;
    }
}
*/

