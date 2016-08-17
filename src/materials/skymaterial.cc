
#include "skymaterial.h"

using namespace r1h;

SkyMaterial::SkyMaterial():
    mulColor(1.0),
    addColor(0.0)
{
	transform.setIdentity();
    textures.resize(1);
}

SkyMaterial::~SkyMaterial() {
}

Color SkyMaterial::skyColor(const Ray &ray) const {
	Color ret;
	
    Texture *tex = Material::getTexture(0);
	if(tex != nullptr) {
		Vector3 dirv = Matrix4::mulV3(transform, ray.direction);
		dirv.normalize();

		Vector3 coord;
		coord.x = atan2(dirv.x, -dirv.z) / (kPI * 2.0) + 0.5;
		coord.y = acos(dirv.y) / kPI;
		coord.z = 0.0;
        
		ret = tex->sample(coord);
    } else {
        ret.set(0.0, 0.0, 0.0);
    }
	
    return Vector3::mul(ret, mulColor) + addColor;
}

void SkyMaterial::setTexture(TextureRef tex) {
    Material::setTexture(0, tex);
}
void SkyMaterial::setColor(const Color& col) {
	Material::setColorTexture(0, col);
}
void SkyMaterial::setMultiplyColor(const Color& col) {
    mulColor = col;
}
void SkyMaterial::setAddColor(const Color& col) {
    addColor = col;
}

void SkyMaterial::setTransform(const Matrix4& m) {
	transform = m;
}
/*
Color SkyMaterial::getReflectance(const SceneObject *obj, const Intersection &isect) const {
	return Color(0.0);
}
*/

Color SkyMaterial::getEmittance(const FinalIntersection &isect) const {
    return Color(0.0);
}

R1hFPType SkyMaterial::getTerminationProbability(const FinalIntersection &isect) const {
    return 0.0;
}

void SkyMaterial::makeNextSampleRays(Renderer::Context* cntx, const FinalIntersection &isect, const int depth) const {
}
