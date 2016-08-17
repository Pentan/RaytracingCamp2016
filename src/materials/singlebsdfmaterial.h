
#ifndef R1H_SINGLE_BSDF_MATERIAL_H
#define R1H_SINGLE_BSDF_MATERIAL_H

#include <vector>

#include "material.h"

namespace r1h {

class SingleBSDFMaterial : public Material{
public:
    enum TexTypes {
        kReflectance,
        kEmittance,
        kNormalMap,
        
        kNumTextures
    };
    
public:
	SingleBSDFMaterial(BSDFRef bsdf=nullptr);
	~SingleBSDFMaterial();
	
    /*
	void setReflectanceColor(const Color col);
	void setReflectanceTexture(TextureRef tex);
	Texture* getReflectanceTexture() const;
	
	void setEmittanceColor(const Color col);
	void setEmittanceTexture(TextureRef tex);
	Texture* getEmittanceTexture() const;
	
    void setNormalTexture(TextureRef tex);
    Texture* getNormalTexture() const;
    */
    
	void setBSDF(BSDFRef newbsdf);
	BSDF* getBSDF() const;
	
    virtual Color getEmittance(const FinalIntersection &isect) const;
    virtual R1hFPType getTerminationProbability(const FinalIntersection &isect) const;
	virtual void makeNextSampleRays(Renderer::Context* cntx, const FinalIntersection &isect, const int depth) const;
	
    virtual Vector3 getShadingNormal(const FinalIntersection &isect) const;
    
private:
	BSDFRef bsdf;
	
    /*
	TextureRef reflectanceTex;
    TextureRef emittanceTex;
    TextureRef normalTex;
    */
};

}

#endif
