
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
    cntx->emitRay(smpl.getRay(), reflectance, bsdf->getType());
}

Color SingleBSDFMaterial::evalShadowRay(Renderer::Context* cntx, const Ray &shadowray, const FinalIntersection &isect) const {
    if((bsdf->getType() & (BSDF::kSpecular | BSDF::kRefraction)) != 0) {
        return Color(0.0);
    }
    
    const Ray &inray = cntx->getCurrentInsidentRay();
    BSDF::Sample insmpl;
    insmpl.position = isect.position;
    insmpl.normal = isect.shadingNormal;
    insmpl.direction = inray.direction;
    
    BSDF::Sample outsmpl;
    outsmpl.position = isect.position;
    outsmpl.normal = isect.shadingNormal;
    outsmpl.direction = shadowray.direction;
    
    Color f = bsdf->evaluate(insmpl, outsmpl);
    Texture *tex = getTexture(kReflectance);
    Color reflectance = tex->sample(&isect);
    //R1hFPType p = bsdf->probabilityForSample(insmpl, outsmpl);
    
    return Color::mul(f, reflectance);// / p;
}

Vector3 SingleBSDFMaterial::getShadingNormal(const FinalIntersection &isect) const {
    Texture *tex = getTexture(kNormalMap);
    if(tex == nullptr) {
        return isect.hitNormal;
    } else {
        return tex->sampleAsVector(&isect);
    }
}
