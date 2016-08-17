#ifndef R1H_FULLREFLECTIONBSDF_H
#define R1H_FULLREFLECTIONBSDF_H

#include "bsdf.h"

namespace r1h {

class FullReflectionBSDF : public BSDF {
public:
    FullReflectionBSDF();
    ~FullReflectionBSDF();
    
    int getType() const { return kSpecular; };
    
    Sample getSample(Renderer::Context *cntx, const FinalIntersection &isect);
    R1hFPType evaluate(const Sample& insident, const Sample& outgoing);
    R1hFPType probabilityForSample(const Sample& insident, const Sample& smpl);
};

}
#endif
