
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
    Sample smpl;
    smpl.position = isect.position;
    smpl.normal = (idotn < 0.0)? isect.shadingNormal : (isect.shadingNormal * -1.0);
    smpl.direction = inray.direction - isect.shadingNormal * 2.0 * idotn;
    smpl.bsdf = 1.0 / std::abs(idotn);
    smpl.pdf = 1.0;
    
    return smpl;
}
R1hFPType FullReflectionBSDF::evaluate(const Sample& insident, const Sample& outgoing) {
    const Vector3 &norm = outgoing.normal;
    R1hFPType indot = Vector3::dot(insident.direction * -1.0, norm);
    R1hFPType outdot = Vector3::dot(outgoing.direction, norm);
    
    return (std::abs(indot - outdot) < kEPS)? 1.0 / std::abs(outdot) : 0.0;
}
R1hFPType FullReflectionBSDF::probabilityForSample(const Sample& insident, const Sample& smpl) {
    return 1.0;
}
