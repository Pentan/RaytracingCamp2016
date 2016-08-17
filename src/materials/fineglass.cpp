
#include "fineglass.h"

using namespace r1h;

FineGlassMaterial::FineGlassMaterial(const R1hFPType ior):
	Material()
{
    reflectionBSDF = new FullReflectionBSDF();
    refractionBSDF = new FullRefractionBSDF(ior);
    textures.resize(kNumTextures);
}

FineGlassMaterial::~FineGlassMaterial() {
    delete reflectionBSDF;
    delete refractionBSDF;
}
/*
void FineGlassMaterial::setReflectanceColor(const Color col) {
	reflectanceTex = TextureRef(new ConstantColorTexture(col));
}
void FineGlassMaterial::setReflectanceTexture(TextureRef tex) {
	reflectanceTex = tex;
}
Texture* FineGlassMaterial::getReflectanceTexture() const {
	return reflectanceTex.get();
}

void FineGlassMaterial::setTransmittanceColor(const Color col) {
    transmittanceTex = TextureRef(new ConstantColorTexture(col));
}
void FineGlassMaterial::setTransmittanceTexture(TextureRef tex) {
    transmittanceTex = tex;
}
Texture* FineGlassMaterial::getTransmittanceTexture() const {
    return transmittanceTex.get();
}

void FineGlassMaterial::setEmittanceColor(const Color col) {
	emittanceTex = TextureRef(new ConstantColorTexture(col));
}
void FineGlassMaterial::setEmittanceTexture(TextureRef tex) {
	emittanceTex = tex;
}
Texture* FineGlassMaterial::getEmittanceTexture() const {
	return emittanceTex.get();
}
void FineGlassMaterial::setNormalTexture(TextureRef tex) {
    normalTex = tex;
}

Texture* FineGlassMaterial::getNormalTexture() const {
    return normalTex.get();
}
*/

Color FineGlassMaterial::getEmittance(const FinalIntersection &isect) const {
    Texture *tex = getTexture(kEmittance);
    return tex->sample(&isect);
}

R1hFPType FineGlassMaterial::getTerminationProbability(const FinalIntersection &isect) const {
    Texture *reftex = getTexture(kReflectance);
    Color reflectance = reftex->sample(&isect);
    R1hFPType refmax = reflectance.getMaxComponent();
    
    Texture *trtex = getTexture(kTransmittance);
    Color transmittance = trtex->sample(&isect);
    R1hFPType trmax = transmittance.getMaxComponent();
    
    return std::max(refmax, trmax);
}


void FineGlassMaterial::setIor(R1hFPType ior) {
    refractionBSDF->setIor(ior);
}

R1hFPType FineGlassMaterial::getIor() const {
    return refractionBSDF->getIor();
}

void FineGlassMaterial::makeNextSampleRays(Renderer::Context* cntx, const FinalIntersection &isect, const int depth) const {
    // Fresnel reflection (Schlick's approximation)
    const Ray &inray = cntx->getCurrentInsidentRay();
    
    Texture *reftex = getTexture(kReflectance);
    Texture *trtex = getTexture(kTransmittance);
    Color reflectance = reftex->sample(&isect);
    Color transmittance = trtex->sample(&isect);
    
    BSDF::Sample tr_smpl = refractionBSDF->getSample(cntx, isect);
    
    // full reflection case
    if((tr_smpl.flag & FullRefractionBSDF::kReflected) != 0) {
        cntx->emitRay(tr_smpl.getRay(), reflectance);
        return;
    }
    
    // transmit
    BSDF::Sample ref_smpl = reflectionBSDF->getSample(cntx, isect);
    
    const R1hFPType ior = refractionBSDF->getIor();
    const R1hFPType nsubn = 1.0 - ior;
    const R1hFPType naddn = 1.0 + ior;
    const R1hFPType F0 = (nsubn * nsubn) / (naddn * naddn);
    
    R1hFPType rdotn;
    if((tr_smpl.flag & FullRefractionBSDF::kInTo) == 0 && ior < 1.0) {
        // use outgoing angle
        rdotn = Vector3::dot(tr_smpl.direction, tr_smpl.normal);
    } else {
        // use insident angle
        rdotn = Vector3::dot(inray.direction, isect.shadingNormal);
    }
    const R1hFPType Re = F0 + (1.0 - F0) * pow(1.0 - std::abs(rdotn), 5.0);
    const R1hFPType Tr = 1.0 - Re;
    
    if(depth > 2) {
        // trace one
        const R1hFPType prob = 0.25 + 0.5 * Re;
        if(cntx->random.next01() < prob) {
            // reflect
            cntx->emitRay(ref_smpl.getRay(), reflectance * (Re / prob));
        } else {
            // transmit
            cntx->emitRay(tr_smpl.getRay(), transmittance * (Tr / ( 1.0 - prob)));
        }
    } else {
        // trace both
        cntx->emitRay(ref_smpl.getRay(), reflectance * Re);
        cntx->emitRay(tr_smpl.getRay(), transmittance * Tr);
    }
}

Vector3 FineGlassMaterial::getShadingNormal(const FinalIntersection &isect) const {
    Texture *tex = getTexture(kNormalMap);
    
    if(tex == nullptr) {
        return isect.hitNormal;
    } else {
        // normal map
        Color normcol = tex->sample(&isect);
        Vector3 ret;
        
        // FIXME
        ret.x = normcol.r * 2.0 - 1.0;
        ret.y = normcol.g * 2.0 - 1.0;
        ret.z = normcol.b * 2.0 - 1.0;
        
        return ret;
    }
}
