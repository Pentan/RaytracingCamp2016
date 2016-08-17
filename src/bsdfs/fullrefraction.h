#ifndef R1H_FULLREFRACTIONBSDF_H
#define R1H_FULLREFRACTIONBSDF_H

#include "bsdf.h"

namespace r1h {

class FullRefractionBSDF : public BSDF {
public:
    enum Flag {
        kReflected  = 1,
        kInTo       = 2
    };
    
public:
	FullRefractionBSDF(const R1hFPType iior=1.5);
    ~FullRefractionBSDF();
    
    int getType() const { return kTransmit | kRefraction | kSpecular; };
    
    Sample getSample(Renderer::Context *cntx, const FinalIntersection &isect);
    R1hFPType evaluate(const Sample& insident, const Sample& outgoing);
    R1hFPType probabilityForSample(const Sample& insident, const Sample& smpl);
    
    void setIor(R1hFPType nior);
    R1hFPType getIor() const;
	
private:
	R1hFPType ior;
};

}
#endif
