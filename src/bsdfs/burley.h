#ifndef R1H_BURLEYBSDF_H
#define R1H_BURLEYBSDF_H

#include "bsdf.h"

namespace r1h {

class BurleyBSDF : public BSDF {
public:
    BurleyBSDF();
    ~BurleyBSDF();
    
    int getType() const { return kDiffuse; };
    
    //void makeNextRays(const Ray &ray, const Intersection &hp, const int depth, Random *rnd, std::vector<Ray> *outvecs);
    Sample getSample(Renderer::Context *cntx, const FinalIntersection &isect);
    Sample getSample(Renderer::Context *cntx, const FinalIntersection &hp, const R1hFPType rough);
    R1hFPType evaluate(const Sample& insident, const Sample& outgoing);
    R1hFPType probabilityForSample(const Sample& insident, const Sample& smpl);
    
    void setRoughness(R1hFPType r);
    R1hFPType getRoughness() const;
    
private:
    R1hFPType roughness;
};

}
#endif
