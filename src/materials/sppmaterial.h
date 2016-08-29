
#ifndef R1H_SINPLE_PHYSICALY_PLAUSIBLE_MATERIAL_H
#define R1H_SINPLE_PHYSICALY_PLAUSIBLE_MATERIAL_H

#include <vector>

#include "material.h"

namespace r1h {

class SimplePhysicalMaterial : public Material{
public:
    enum TexTypes {
        kAlbedo,
        kRoughness,
        kMetalness,
        kNormalMap,
        kEmittance,
        
        kNumTextures
    };
    
public:
	SimplePhysicalMaterial(BSDFRef bsdf=nullptr);
	~SimplePhysicalMaterial();
    
	BurleyBSDF* getDiffuseBSDF() const;
    GGXBSDF* getSpecularBSDF() const;
	
    virtual Color getEmittance(const FinalIntersection &isect) const;
    virtual R1hFPType getTerminationProbability(const FinalIntersection &isect) const;
	virtual void makeNextSampleRays(Renderer::Context* cntx, const FinalIntersection &isect, const int depth) const;
	virtual Color evalShadowRay(Renderer::Context* cntx, const Ray &shadowray, const FinalIntersection &isect) const;
    
    virtual Vector3 getShadingNormal(const FinalIntersection &isect) const;
    
    void setDiffuseRoughness(R1hFPType r);
    void setSpecularIor(R1hFPType ior);
    
private:
    BSDFRef diffuseBSDF;
    BSDFRef specularBSDF;
	
    R1hFPType diffuseRoughness;
    R1hFPType specularIor;
};

}

#endif
