#include <iostream>
#include "burley.h"

using namespace r1h;

BurleyBSDF::BurleyBSDF():
    roughness(0.5)
{
}

BurleyBSDF::~BurleyBSDF() {
}

BSDF::Sample BurleyBSDF::getSample(Renderer::Context *cntx, const FinalIntersection &isect) {
    return getSample(cntx, isect, roughness);
}

BSDF::Sample BurleyBSDF::getSample(Renderer::Context *cntx, const FinalIntersection &isect, const R1hFPType rough) {
    // importance sampling using cosine distoribution.
    BSDF::Sample ret;
    
    const R1hFPType r1 = 2.0 * kPI * cntx->random.next01();
    const R1hFPType r2 = cntx->random.next01();
    const R1hFPType r2s = sqrt(r2);
    
    Vector3 dir;
    dir.x = cos(r1) * r2s;
    dir.y = sin(r1) * r2s;
    dir.z = sqrt(1.0 - r2);
    
    R1hFPType ndotd = dir.z; // dir.z == dot(normal, direction)
    
    dir = Matrix4::mulV3(isect.inverseTangentSpaceBasis, dir);
    
    const Ray& inray = cntx->getCurrentInsidentRay();
    Vector3 norm;
    if(Vector3::dot(inray.direction, isect.shadingNormal) < 0.0) {
        // from front
        norm = isect.shadingNormal;
    } else {
        // from back
        dir = dir * -1.0;
        norm = isect.shadingNormal * -1.0;
    }
    ret.position = isect.position;
    ret.direction = dir;
    ret.normal = norm;
    if(BSDF::isCurrectBRDF(inray.direction, dir, isect.geometryNormal)) {
        Vector3 vi = inray.direction * -1.0;
        Vector3 vl = dir;
        Vector3 vh = (vi + vl) * 0.5;
        vh.normalize();
        
        R1hFPType costl = Vector3::dot(norm, vl);
        R1hFPType costv = Vector3::dot(norm, vi);
        R1hFPType costd = Vector3::dot(vh, vl);
        
        R1hFPType FD90 = 0.5 + 2.0 * costd * costd * rough;
        R1hFPType L = 1.0 + (FD90 - 1.0) * std::pow(1.0 - costl, 5.0);
        R1hFPType V = 1.0 + (FD90 - 1.0) * std::pow(1.0 - costv, 5.0);
        
        ret.bsdf = L * V / kPI;
        
    } else {
        ret.bsdf = 0.0;
    }
    ret.pdf = ndotd / kPI;
    
    return ret;
}

R1hFPType BurleyBSDF::evaluate(const Sample& insident, const Sample& outgoing) {
    /*
    R1hFPType idot = Vector3::dot(insident.direction, insident.normal);
    R1hFPType odot = Vector3::dot(outgoing.direction, outgoing.normal);
    R1hFPType ndot = Vector3::dot(insident.normal, outgoing.normal);
    
    if(idot * odot < 0.0 && (1.0 - ndot) < kEPS) {
        // same side
        return 1.0 / kPI;
    } else {
        // another side
        return 0.0;
    }
     */
    // TODO
    return 0.0;
}

R1hFPType BurleyBSDF::probabilityForSample(const Sample& insident, const Sample& smpl) {
    return Vector3::dot(smpl.direction, smpl.normal) / kPI;
}

void BurleyBSDF::setRoughness(R1hFPType r) {
    roughness = r;
}

R1hFPType BurleyBSDF::getRoughness() const {
    return roughness;
}

