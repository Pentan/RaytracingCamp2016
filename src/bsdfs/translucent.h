#ifndef R1H_TRANSLUCENTBSDF_H
#define R1H_TRANSLUCENTBSDF_H

#include "bsdf.h"

namespace r1h {

class TranslucentBSDF : public BSDF {
public:
    TranslucentBSDF();
    ~TranslucentBSDF();
    
    int getType() const { return kTransmit; };
    
    Sample getSample(Renderer::Context *cntx, const FinalIntersection &isect);
    R1hFPType evaluate(const Sample& insident, const Sample& outgoing);
    R1hFPType probabilityForSample(const Sample& insident, const Sample& smpl);
};

}
#endif
