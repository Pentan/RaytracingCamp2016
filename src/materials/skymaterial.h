#ifndef R1H_SKY_MATERIAL_H
#define R1H_SKY_MATERIAL_H

#include "material.h"
#include <textures/texture_include.h>

namespace r1h {

class SkyMaterial : public Material {
public:
	SkyMaterial();
	virtual ~SkyMaterial();
	
	Color skyColor(const Ray &ray) const;
	
	void setTexture(TextureRef tex);
    void setColor(const Color& col);
    void setMultiplyColor(const Color& col);
    void setAddColor(const Color& col);
    
	void setTransform(const Matrix4& m);
	
	// These methods are noop
    virtual Color getEmittance(const FinalIntersection &isect) const;
    virtual R1hFPType getTerminationProbability(const FinalIntersection &isect) const;
    virtual void makeNextSampleRays(Renderer::Context* cntx, const FinalIntersection &isect, const int depth) const;
    
private:
    Color mulColor;
    Color addColor;
	Matrix4 transform;
};

typedef std::shared_ptr<SkyMaterial> SkyMaterialRef;

}

#endif
