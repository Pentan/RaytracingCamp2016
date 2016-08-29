
#include "material.h"

using namespace r1h;

/*
Material::ShadingInfo Material::makeShadingInfo(const Ray& inray, const Vector3& shdnorm, const Intersection &isect) const {
    ShadingInfo si;
    si.intersection = &isect;
    si.insidentVec = inray.direction * -1.0;
    si.shadingNormal = shdnorm;
    
    Vector3 basisx = isect.tangent;
    Vector3 basisy = si.shadingNormal;
    Vector3 basisz;
    
    if(!basisx.isZero()) {
        // tangent is defined
        basisz = Vector3::cross(basisx, basisy);
        basisz.normalize();
    } else {
        // tangent is not defined
        vqm::VectorComponent vc;
        basisy.getMaxComponent(&vc);
        if(vc == vqm::VectorComponent::kY) {
            basisx.set(1.0, 0.0, 0.0);
        } else {
            basisx.set(0.0, 1.0, 0.0);
        }
        
        basisz = Vector3::cross(basisx, basisy);
        basisz.normalize();
        basisx = Vector3::cross(basisy, basisz);
        basisx.normalize();
    }
    
    si.tangentSpaceBasis.setBasis(basisx, basisy, basisz);
    si.tangentToGlobalMatrix = Matrix4::transposed(si.tangentSpaceBasis);
    
    return si;
}
*/

Vector3 Material::getShadingNormal(const FinalIntersection &isect) const {
    return isect.shadingNormal;
}

void Material::setTexture(int i, TextureRef texref) {
    textures[i] = texref;
}
void Material::setColorTexture(int i, const Color& col) {
    textures[i] = TextureRef(new ConstantColorTexture(col));
}
Texture* Material::getTexture(int i) const {
    return textures[i].get();
}

bool Material::isLight() const {
    return false;
}
