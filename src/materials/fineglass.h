
#ifndef R1H_FINE_GLASS_MATERIAL_H
#define R1H_FINE_GLASS_MATERIAL_H

#include <vector>
#include "material.h"

namespace r1h {

class FineGlassMaterial : public Material{
public:
    enum TexTypes {
        kReflectance,
        kTransmittance,
        kEmittance,
        kNormalMap,
        
        kNumTextures
    };
    
public:
	FineGlassMaterial(const R1hFPType ior=1.5);
	~FineGlassMaterial();
	
    /*
	void setReflectanceColor(const Color col);
	void setReflectanceTexture(TextureRef tex);
	Texture* getReflectanceTexture() const;
    
    void setTransmittanceColor(const Color col);
    void setTransmittanceTexture(TextureRef tex);
    Texture* getTransmittanceTexture() const;
	
	void setEmittanceColor(const Color col);
	void setEmittanceTexture(TextureRef tex);
	Texture* getEmittanceTexture() const;
	
    void setNormalTexture(TextureRef tex);
    Texture* getNormalTexture() const;
    */
    
    void setIor(R1hFPType ior);
    R1hFPType getIor() const;
    
    virtual Color getEmittance(const FinalIntersection &isect) const;
    virtual R1hFPType getTerminationProbability(const FinalIntersection &isect) const;
    virtual void makeNextSampleRays(Renderer::Context* cntx, const FinalIntersection &isect, const int depth) const;
    virtual Color evalShadowRay(Renderer::Context* cntx, const Ray &shadowray, const FinalIntersection &isect) const;
    
    virtual Vector3 getShadingNormal(const FinalIntersection &isect) const;
    
private:
    FullReflectionBSDF *reflectionBSDF;
    FullRefractionBSDF *refractionBSDF;
    
    /*
    TextureRef reflectanceTex;
    TextureRef transmittanceTex;
    TextureRef emittanceTex;
    TextureRef normalTex;
     */
};

}

#endif
