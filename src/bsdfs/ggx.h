#ifndef R1H_GGXBSDF_H
#define R1H_GGXBSDF_H

#include "bsdf.h"

namespace r1h {

class GGXBSDF : public BSDF {
public:
    GGXBSDF();
    ~GGXBSDF();
    
    int getType() const { return kGlossy; };
    
    //void makeNextRays(const Ray &ray, const Intersection &hp, const int depth, Random *rnd, std::vector<Ray> *outvecs);
    Sample getSample(Renderer::Context *cntx, const FinalIntersection &isect);
    Sample getSample(Renderer::Context *cntx, const FinalIntersection &isect, const R1hFPType rough, const R1hFPType ior);
    R1hFPType evaluate(const Sample& insident, const Sample& outgoing);
    R1hFPType probabilityForSample(const Sample& insident, const Sample& smpl);
    
    void setRoughness(R1hFPType r);
    R1hFPType getRoughness() const;
    
    void setFresnelIor(R1hFPType i);
    R1hFPType getFresnelIor() const;
    
private:
    R1hFPType roughness;
    R1hFPType fresnelIor;
};

}
#endif
