#include <iostream>
#include "lambert.h"

using namespace r1h;

LambertBSDF::LambertBSDF() {
}

LambertBSDF::~LambertBSDF() {
}

/*
void LambertBSDF::makeNextRays(const Ray &ray, const Intersection &hp, const int depth, Random *rnd, std::vector<Ray> *outvecs) {
	Vector3 w, u, v;
	w = hp.orientingNormal(ray);
	
	if(fabs(w.x) > kEPS) {
		u = Vector3::normalized(Vector3::cross(Vector3(0.0, 1.0, 0.0), w));
	} else {
		u = Vector3::normalized(Vector3::cross(Vector3(1.0, 0.0, 0.0), w));
	}
	v = Vector3::cross(w, u);
	
	// importance sampling using cosine.
	const double r1 = 2.0 * kPI * rnd->next01();
	const double r2 = rnd->next01();
	const double r2s = sqrt(r2);
	Vector3 dir = Vector3::normalized((
		u * cos(r1) * r2s +
		v * sin(r1) * r2s +
		w * sqrt(1.0 - r2)
	));
	
	Ray nextray;
	nextray = Ray(hp.position, dir, Color(1.0, 1.0, 1.0));
	outvecs->push_back(nextray);
}
*/

BSDF::Sample LambertBSDF::getSample(Renderer::Context *cntx, const FinalIntersection &isect) {
    // importance sampling using cosine distoribution.
    BSDF::Sample ret;
    
    const R1hFPType r1 = 2.0 * kPI * cntx->random.next01();
    const R1hFPType r2 = cntx->random.next01();
    const R1hFPType r2s = sqrt(r2);
    
    Vector3 dir;
    dir.x = cos(r1) * r2s;
    dir.y = sin(r1) * r2s;
    dir.z = sqrt(1.0 - r2);
    
    R1hFPType ndotd = dir.z; // dir.z == dot(normal, direction)
    
    dir = Matrix4::mulV3(isect.inverseTangentSpaceBasis, dir);
    
    const Ray& inray = cntx->getCurrentInsidentRay();
    if(Vector3::dot(inray.direction, isect.shadingNormal) < 0.0) {
        // from front
        ret.position = isect.position;
        ret.direction = dir;
        ret.normal = isect.shadingNormal;
    } else {
        // from back
        ret.position = isect.position;
        ret.direction = dir * -1.0;
        ret.normal = isect.shadingNormal * -1.0;
    }
    ret.bsdf = 1.0 / kPI;
    ret.pdf = ndotd / kPI;
    
    return ret;
}

R1hFPType LambertBSDF::evaluate(const Sample& insident, const Sample& outgoing) {
    return 1.0 / kPI;
}

R1hFPType LambertBSDF::probabilityForSample(const Sample& insident, const Sample& smpl) {
    return Vector3::dot(smpl.direction, smpl.normal) / kPI;
}

