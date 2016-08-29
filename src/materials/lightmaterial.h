
#ifndef R1H_LIGHT_MATERIAL_H
#define R1H_LIGHT_MATERIAL_H

#include <vector>

#include "material.h"

namespace r1h {

class LightMaterial : public Material{
public:
    enum TexTypes {
        kEmittance,
        kNormalMap,
        kNumTextures
    };
public:
	LightMaterial();
	~LightMaterial();
    
    void setColorBias(const Color &col);
    Color getColorBias() const;
    
    virtual Color getEmittance(const FinalIntersection &isect) const;
    virtual R1hFPType getTerminationProbability(const FinalIntersection &isect) const;
	virtual void makeNextSampleRays(Renderer::Context* cntx, const FinalIntersection &isect, const int depth) const;
	virtual Color evalShadowRay(Renderer::Context* cntx, const Ray &shadowray, const FinalIntersection &isect) const;
    
    virtual Vector3 getShadingNormal(const FinalIntersection &isect) const;
    
    virtual bool isLight() const;
private:
    Color colorBias;
};

}

#endif
