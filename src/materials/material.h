#ifndef R1H_ABSTRACT_MATERIAL_H
#define R1H_ABSTRACT_MATERIAL_H

#include <memory>
#include <vector>

#include <r1htypes.h>
#include <intersection.h>
//#include <sceneobject.h>
#include <renderer.h>
#include <random.h>
#include <asset.h>
#include <bsdfs/bsdf_include.h>
#include <textures/texture_include.h>

namespace r1h {

class SceneObject;

class Material : public Asset {
public:
    Material() {}
    virtual ~Material() {}
	
    virtual Color getEmittance(const FinalIntersection &isect) const = 0;
    virtual R1hFPType getTerminationProbability(const FinalIntersection &isect) const = 0;
    virtual void makeNextSampleRays(Renderer::Context* cntx, const FinalIntersection &isect, const int depth) const = 0;
    
    virtual Vector3 getShadingNormal(const FinalIntersection &isect) const;
    
    virtual void setTexture(int i, TextureRef texref);
    virtual void setColorTexture(int i, const Color& col);
    virtual Texture* getTexture(int i) const;
    
protected:
    std::vector<TextureRef> textures;
};

typedef std::shared_ptr<Material> MaterialRef;

/// Stub
class StubMaterial : public Material {
public:
	StubMaterial(const std::string& refid) : Material() {
		assetId = refid;
		assetType = Asset::Type::kStub;
	}
	~StubMaterial() {}
	
	virtual Color getEmittance(const SceneObject *obj, const FinalIntersection &isect) const { return Color(1.0, 0.0, 0.0); };
    virtual R1hFPType getTerminationProbability(const SceneObject *obj, const FinalIntersection &isect) const { return 0.0; };
    virtual void makeNextSampleRays(Renderer::Context* cntx, const FinalIntersection &isect, const int depth) const {};
};

}

#endif
