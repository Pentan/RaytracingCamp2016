
#include "sppmaterial.h"

using namespace r1h;

SimplePhysicalMaterial::SimplePhysicalMaterial(BSDFRef bsdf):
	Material(),
    diffuseRoughness(0.5),
    specularIor(1.5)
{
    textures.resize(kNumTextures);
    diffuseBSDF = BSDFRef(new BurleyBSDF());
    specularBSDF = BSDFRef(new GGXBSDF());
}

SimplePhysicalMaterial::~SimplePhysicalMaterial() {
}

BurleyBSDF* SimplePhysicalMaterial::getDiffuseBSDF() const {
    return reinterpret_cast<BurleyBSDF*>(diffuseBSDF.get());
}
GGXBSDF* SimplePhysicalMaterial::getSpecularBSDF() const {
    return reinterpret_cast<GGXBSDF*>(specularBSDF.get());
}

R1hFPType SimplePhysicalMaterial::getTerminationProbability(const FinalIntersection &isect) const {
    Texture *albedtex = getTexture(kAlbedo);
    Color albed = albedtex->sample(&isect);
    R1hFPType albedmax = albed.getMaxComponent();
    
    Texture *roughtex = getTexture(kRoughness);
    Color gloss = Vector3(1.0) - roughtex->sample(&isect);
    R1hFPType glossmax = gloss.getMaxComponent();
    
    return std::max(albedmax, glossmax);
}

Color SimplePhysicalMaterial::getEmittance(const FinalIntersection &isect) const {
	Texture *tex = getTexture(kEmittance);
    return tex->sample(&isect);
}

void SimplePhysicalMaterial::makeNextSampleRays(Renderer::Context* cntx, const FinalIntersection &isect, const int depth) const {
    Color albedo = getTexture(kAlbedo)->sample(&isect);
    R1hFPType roughness = getTexture(kRoughness)->sample(&isect).r;
    R1hFPType metalness = getTexture(kMetalness)->sample(&isect).r;
    
    Color diffuseweight = albedo * (1.0 - metalness);
    Color specularweight = Vector3(1.0 - metalness) + albedo * metalness;
    specularweight = specularweight * (1.0 - roughness);
    
    BurleyBSDF *diffusebsdf = getDiffuseBSDF();
    GGXBSDF *specbsdf = getSpecularBSDF();
    
    if(depth > 1) {
        // diffuse or specular
        R1hFPType prob = 0.25 + 0.5 * std::max(metalness, roughness);
        
        if(cntx->random.next01() > prob) {
            BSDF::Sample smpl = specbsdf->getSample(cntx, isect, roughness, specularIor);
            cntx->emitRay(smpl.getRay(), specularweight * (1.0 / prob), specbsdf->getType());
        } else {
            BSDF::Sample smpl = diffusebsdf->getSample(cntx, isect, diffuseRoughness);
            cntx->emitRay(smpl.getRay(), diffuseweight * (1.0 / (1.0 - prob)), diffusebsdf->getType());
        }
    } else {
        // both
        BSDF::Sample smpl;
        smpl = specbsdf->getSample(cntx, isect, roughness, specularIor);
        cntx->emitRay(smpl.getRay(), specularweight, specularBSDF->getType());
        
        smpl = diffusebsdf->getSample(cntx, isect, diffuseRoughness);
        cntx->emitRay(smpl.getRay(), diffuseweight, diffuseBSDF->getType());
    }
}

Color SimplePhysicalMaterial::evalShadowRay(Renderer::Context* cntx, const Ray &shadowray, const FinalIntersection &isect) const {
#if 0
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
#else
    return Color(0.0);
# endif
}

Vector3 SimplePhysicalMaterial::getShadingNormal(const FinalIntersection &isect) const {
    Texture *tex = getTexture(kNormalMap);
    if(tex == nullptr) {
        return isect.hitNormal;
    } else {
        return tex->sampleAsVector(&isect);
    }
}

void SimplePhysicalMaterial::setDiffuseRoughness(R1hFPType r) {
    diffuseRoughness = r;
}

void SimplePhysicalMaterial::setSpecularIor(R1hFPType ior) {
    specularIor = ior;
}

