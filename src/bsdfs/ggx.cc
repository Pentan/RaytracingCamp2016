#include <cassert>
#include <iostream>
#include "ggx.h"

using namespace r1h;

#define kMinAlphaG  1e-4

namespace {
    
    R1hFPType Xt(R1hFPType x) {
        return (x > 0.0)? 1.0 : 0.0;
    }
    
    R1hFPType F(const Vector3 &i, const Vector3 &n, const R1hFPType ior) {
        R1hFPType nsubn = 1.0 - ior;
        R1hFPType naddn = 1.0 + ior;
        R1hFPType F0 = (nsubn * nsubn) / (naddn * naddn);
        R1hFPType idotn = Vector3::dot(i, n);
        return F0 + (1.0 - F0) * std::pow((1.0 - idotn), 5.0);
    }
    
    R1hFPType G(const Vector3 &v, const Vector3 &m, const Vector3 &n, const R1hFPType ag) {
        R1hFPType vdotn = Vector3::dot(v, n);
        R1hFPType vdotm = Vector3::dot(v, m);
        R1hFPType tv2 = (1.0 - vdotn * vdotn) / (vdotn * vdotn);
        R1hFPType sqrt1a2t2 = std::sqrt(1.0 + ag * ag * tv2);
        return Xt(vdotm / vdotn) * (2.0 / (1.0 + sqrt1a2t2));
    }
    
    R1hFPType D(const Vector3 &m, const Vector3 &n, const R1hFPType ag) {
        R1hFPType mdotn = Vector3::dot(m, n);
        R1hFPType a2X = ag * ag * Xt(mdotn);
        R1hFPType a2c2s2 = (ag * ag) * (mdotn * mdotn) + (1.0 - mdotn * mdotn);
        return a2X / (kPI * std::pow(a2c2s2, 2.0));
        /*
        R1hFPType pic4 = kPI * std::pow(mdotn, 4.0);
        R1hFPType a2t2 = ag * ag + (1.0 - mdotn * mdotn) / (mdotn * mdotn);
        return a2X / (pic4 * a2t2 * a2t2);
         */
    }
    
}

GGXBSDF::GGXBSDF():
    roughness(0.5),
    fresnelIor(1.5)
{
    setRoughness(roughness);
}

GGXBSDF::~GGXBSDF() {
}

BSDF::Sample GGXBSDF::getSample(Renderer::Context *cntx, const FinalIntersection &isect) {
    return getSample(cntx, isect, roughness, fresnelIor);
}

BSDF::Sample GGXBSDF::getSample(Renderer::Context *cntx, const FinalIntersection &isect, const R1hFPType rough, const R1hFPType ior) {
    // importance sampling
    BSDF::Sample ret;
    
    R1hFPType ag = rough * rough;
    if(ag < kMinAlphaG) { // hack
        ag = kMinAlphaG;
    }
    
    // generate micro normal (= half vector)
    const R1hFPType r1 = cntx->random.next01();
    const R1hFPType r2 = cntx->random.next01();
    
    const R1hFPType theta = std::atan(ag * std::sqrt(r1 / (1.0 - r1)));
    const R1hFPType phi = 2.0 * kPI * r2;
    
    Vector3 hv;
    R1hFPType sintheta = sin(theta);
    hv.x = cos(phi) * sintheta;
    hv.y = sin(phi) * sintheta;
    hv.z = cos(theta);
    
    hv = Matrix4::mulV3(isect.inverseTangentSpaceBasis, hv);
    
    const Ray& inray = cntx->getCurrentInsidentRay();
    Vector3 norm;
    if(Vector3::dot(inray.direction, isect.shadingNormal) < 0.0) {
        // from front
        norm = isect.shadingNormal;
    } else {
        // from back
        hv = hv * -1.0;
        norm = isect.shadingNormal * -1.0;
    }
    
    Vector3 iv = inray.direction * -1.0;
    Vector3 ov = Vector3::dot(iv, hv) * hv * 2.0 - iv;
    
    ret.position = isect.position;
    ret.direction = ov;
    ret.normal = norm;
    
    if(BSDF::isCurrectBRDF(inray.direction, ov, isect.geometryNormal)) {
        // evaluate
        R1hFPType idotn = Vector3::dot(iv, norm);
        R1hFPType odotn = Vector3::dot(ov, norm);
        R1hFPType hdotn = Vector3::dot(hv, norm);
        
        R1hFPType Fih = F(iv, hv, ior);
        R1hFPType Gim = G(iv, hv, norm, ag);
        R1hFPType Gom = G(ov, hv, norm, ag);
        R1hFPType Dm = D(hv, norm, ag);
        R1hFPType A = (4.0 * std::abs(idotn * odotn));
        
        ret.bsdf = Fih * (Gim * Gom) * Dm / A;
        
        R1hFPType hjacobian = 1.0 / (4.0 * std::abs(Vector3::dot(ov, hv)));
        ret.pdf = D(hv, norm, ag) * std::abs(hdotn) * hjacobian;
        
    } else {
        ret.bsdf = 0.0;
        ret.pdf = 1.0;
    }
    
    return ret;
}

R1hFPType GGXBSDF::evaluate(const Sample& insident, const Sample& outgoing) {
    // TODO
    return 0.0;
}

R1hFPType GGXBSDF::probabilityForSample(const Sample& insident, const Sample& smpl) {
    return Vector3::dot(smpl.direction, smpl.normal) / kPI;
}

void GGXBSDF::setRoughness(R1hFPType r) {
    roughness = r;
}

R1hFPType GGXBSDF::getRoughness() const {
    return roughness;
}

void GGXBSDF::setFresnelIor(R1hFPType i) {
    fresnelIor = i;
}

R1hFPType GGXBSDF::getFresnelIor() const {
    return fresnelIor;
}

