#include <cassert>
#include "fullreflection.h"

using namespace r1h;

FullReflectionBSDF::FullReflectionBSDF()
{
}

FullReflectionBSDF::~FullReflectionBSDF()
{
}

BSDF::Sample FullReflectionBSDF::getSample(Renderer::Context *cntx, const FinalIntersection &isect) {
    const Ray inray = cntx->getCurrentInsidentRay();
    R1hFPType idotn = Vector3::dot(inray.direction, isect.shadingNormal);
    const Vector3 vreflect = inray.direction - isect.shadingNormal * 2.0 * idotn;
    
    Sample smpl;
    smpl.position = isect.position;
    smpl.normal = (idotn < 0.0)? isect.shadingNormal : (isect.shadingNormal * -1.0);
    smpl.direction = vreflect;
    if(BSDF::isCurrectBRDF(inray.direction, vreflect, isect.geometryNormal)) {
        smpl.bsdf = 1.0 / std::abs(idotn);
    } else {
        smpl.bsdf = 0.0;
    }
    smpl.pdf = 1.0;
    
    return smpl;
}
R1hFPType FullReflectionBSDF::evaluate(const Sample& insident, const Sample& outgoing) {
    R1hFPType indot = Vector3::dot(insident.direction * -1.0, insident.normal);
    R1hFPType outdot = Vector3::dot(outgoing.direction, outgoing.normal);
    R1hFPType ndot = Vector3::dot(insident.normal, outgoing.normal);
    
    if(indot * outdot < 0.0 || (1.0 - ndot) > kEPS) {
        // another side or different normal
        return 0.0;
    }
    
    return (std::abs(indot - outdot) < kEPS)? 1.0 / std::abs(outdot) : 0.0;
}
R1hFPType FullReflectionBSDF::probabilityForSample(const Sample& insident, const Sample& smpl) {
    return 1.0;
}
