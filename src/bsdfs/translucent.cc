
#include "translucent.h"

using namespace r1h;

TranslucentBSDF::TranslucentBSDF() {
}

TranslucentBSDF::~TranslucentBSDF() {
}

BSDF::Sample TranslucentBSDF::getSample(Renderer::Context *cntx, const FinalIntersection &isect) {
    // importance sampling using cosine distoribution.
    const R1hFPType r1 = 2.0 * kPI * cntx->random.next01();
    const R1hFPType r2 = cntx->random.next01();
    const R1hFPType r2s = sqrt(r2);
    
    BSDF::Sample ret;
    
    Vector3 dir;
    dir.x = cos(r1) * r2s;
    dir.z = sin(r1) * r2s;
    dir.y = sqrt(1.0 - r2);
    
    ret.bsdf = 1.0 / kPI;
    ret.pdf = dir.y / kPI; // dir.y == dot(normal, direction)
    
    //
    dir = Matrix4::mulV3(isect.inverseTangentSpaceBasis, dir);
    
    const Ray& inray = cntx->getCurrentInsidentRay();
    if(Vector3::dot(inray.direction, isect.shadingNormal) < 0.0) {
        // front face
        ret.position = isect.position;
        ret.direction = Vector3::negated(dir);
        ret.normal = Vector3::negated(isect.shadingNormal);
    } else {
        // back face
        ret.position = isect.position;
        ret.direction = dir;
        ret.normal = isect.shadingNormal;
    }
    
    return ret;
}

R1hFPType TranslucentBSDF::evaluate(const Sample& insident, const Sample& outgoing) {
    return 1.0 / kPI;
}

R1hFPType TranslucentBSDF::probabilityForSample(const Sample& insident, const Sample& smpl) {
    return Vector3::dot(smpl.direction, smpl.normal) / kPI;
}



