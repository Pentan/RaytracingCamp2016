
#include "bsdf.h"

using namespace r1h;

#define kSmallOffset    kEPS

Vector3 BSDF::Sample::smallOffsetedPosition() const {
    return position + normal * kSmallOffset;
}

Ray BSDF::Sample::getRay() const {
    Ray ray(smallOffsetedPosition(), direction);
    R1hFPType w = bsdf / pdf * std::abs(Vector3::dot(direction, normal));
    ray.weight.set(w, w, w);
    return ray;
}
