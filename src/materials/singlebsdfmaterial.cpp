
#include "singlebsdfmaterial.h"

using namespace r1h;

SingleBSDFMaterial::SingleBSDFMaterial(BSDFRef bsdf):
	Material(),
	bsdf(bsdf)
{
    textures.resize(kNumTextures);
}

SingleBSDFMaterial::~SingleBSDFMaterial() {
}

/*
void SingleBSDFMaterial::setReflectanceColor(const Color col) {
	reflectanceTex = TextureRef(new ConstantColorTexture(col));
}
void SingleBSDFMaterial::setReflectanceTexture(TextureRef tex) {
	reflectanceTex = tex;
}
Texture* SingleBSDFMaterial::getReflectanceTexture() const {
	return reflectanceTex.get();
}

void SingleBSDFMaterial::setEmittanceColor(const Color col) {
	emittanceTex = TextureRef(new ConstantColorTexture(col));
}
void SingleBSDFMaterial::setEmittanceTexture(TextureRef tex) {
	emittanceTex = tex;
}
Texture* SingleBSDFMaterial::getEmittanceTexture() const {
	return emittanceTex.get();
}
*/
void SingleBSDFMaterial::setBSDF(BSDFRef newbsdf) {
	bsdf = newbsdf;
}

BSDF* SingleBSDFMaterial::getBSDF() const {
	return bsdf.get();
}

R1hFPType SingleBSDFMaterial::getTerminationProbability(const FinalIntersection &isect) const {
    Texture *tex = getTexture(kReflectance);
    Color reflectance = tex->sample(&isect);
    return reflectance.getMaxComponent();
}

Color SingleBSDFMaterial::getEmittance(const FinalIntersection &isect) const {
	Texture *tex = getTexture(kEmittance);
    return tex->sample(&isect);
}

void SingleBSDFMaterial::makeNextSampleRays(Renderer::Context* cntx, const FinalIntersection &isect, const int depth) const {
    Texture *tex = getTexture(kReflectance);
    Color reflectance = tex->sample(&isect);
    
    BSDF::Sample smpl = bsdf->getSample(cntx, isect);
    cntx->emitRay(smpl.getRay(), reflectance);
}

Vector3 SingleBSDFMaterial::getShadingNormal(const FinalIntersection &isect) const {
    Texture *tex = getTexture(kNormalMap);
    if(tex == nullptr) {
        return isect.hitNormal;
    } else {
        return tex->sampleAsVector(&isect);
    }
}
