
#include "bsdf.h"

using namespace r1h;

Vector3 BSDF::Sample::smallOffsetedPosition() const {
    return position + normal * kSmallOffset;
}

Ray BSDF::Sample::getRay() const {
    Ray ray(smallOffsetedPosition(), direction);
    R1hFPType w = bsdf / pdf * std::abs(Vector3::dot(direction, normal));
    ray.weight.set(w, w, w);
    return ray;
}

bool BSDF::isCurrectBRDF(const Vector3& vi, const Vector3& vo, const Vector3& ng) {
    if(Vector3::dot(vi, ng) * Vector3::dot(vo, ng) > 0.0) {
        
        return false;
    }
    return true;
}