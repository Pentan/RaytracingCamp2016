
#include "lightmaterial.h"

using namespace r1h;

LightMaterial::LightMaterial():
	Material(),
    colorBias(1.0)
{
    textures.resize(kNumTextures);
}

LightMaterial::~LightMaterial() {
}

void LightMaterial::setColorBias(const Color &col) {
    colorBias = col;
}
Color LightMaterial::getColorBias() const {
    return colorBias;
}

R1hFPType LightMaterial::getTerminationProbability(const FinalIntersection &isect) const {
    return 0.0;
}

Color LightMaterial::getEmittance(const FinalIntersection &isect) const {
	Texture *tex = getTexture(kEmittance);
    return Vector3::mul(tex->sample(&isect), colorBias);
}

void LightMaterial::makeNextSampleRays(Renderer::Context* cntx, const FinalIntersection &isect, const int depth) const {
    // noop
}

Color LightMaterial::evalShadowRay(Renderer::Context* cntx, const Ray &shadowray, const FinalIntersection &isect) const {
    return Color(0.0);
}

Vector3 LightMaterial::getShadingNormal(const FinalIntersection &isect) const {
    Texture *tex = getTexture(kNormalMap);
    if(tex == nullptr) {
        return isect.hitNormal;
    } else {
        return tex->sampleAsVector(&isect);
    }
}

bool LightMaterial::isLight() const {
    return true;
}
