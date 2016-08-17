#ifndef R1H_LAMBERTBSDF_H
#define R1H_LAMBERTBSDF_H

#include "bsdf.h"

namespace r1h {

class LambertBSDF : public BSDF {
public:
    LambertBSDF();
    ~LambertBSDF();
    
    int getType() const { return kDiffuse; };
    
    //void makeNextRays(const Ray &ray, const Intersection &hp, const int depth, Random *rnd, std::vector<Ray> *outvecs);
    Sample getSample(Renderer::Context *cntx, const FinalIntersection &hp);
    R1hFPType evaluate(const Sample& insident, const Sample& outgoing);
    R1hFPType probabilityForSample(const Sample& insident, const Sample& smpl);
};

}
#endif
