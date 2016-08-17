
#include "fullrefraction.h"

using namespace r1h;

FullRefractionBSDF::FullRefractionBSDF(const R1hFPType iior): ior(iior)
{
}

FullRefractionBSDF::~FullRefractionBSDF()
{
}

/*
void FullRefractionBSDF::makeNextRays(const Ray &ray, const FinalIntersection &hp, const int depth, Random *rnd, std::vector<Ray> *outvecs) {
	const Ray reflection_ray = Ray(hp.position, ray.direction - hp.normal * 2.0 * Vector3::dot(hp.normal, ray.direction));
	const Vector3 orienting_normal = hp.orientingNormal(ray);
	const bool into = Vector3::dot(hp.normal, orienting_normal) > 0.0;
	
	// Snell's law
	const double nc = 1.0;
	const double nt = ior;
	const double nnt = into ? (nc / nt) : (nt / nc);
	const double ddn = Vector3::dot(ray.direction, orienting_normal);
	const double cos2t = 1.0 - nnt * nnt * (1.0 - ddn * ddn);
	
	// full reflection
	if(cos2t < 0.0) {
		Ray nxtray;
		nxtray = reflection_ray;
		nxtray.weight = Color(1.0, 1.0, 1.0);
		outvecs->push_back(nxtray);
		return;
	}
	
	// refraction
	const Ray refraction_ray = Ray(
		hp.position,
		Vector3::normalized(ray.direction * nnt - hp.normal * (into ? 1.0 : -1.0) * (ddn * nnt + sqrt(cos2t)))
		);
	
	// Schlick' Fresnel aproximation
	const double a = nt - nc, b = nt + nc;
	const double R0 = (a * a) / (b * b);
	
	const double c = 1.0 - (into ? -ddn : Vector3::dot(refraction_ray.direction, -1.0 * orienting_normal));
	const double Re = R0 + (1.0 - R0) * pow(c, 5.0);
	const double nnt2 = pow(into ? (nc / nt) : (nt / nc), 2.0);
	const double Tr = (1.0 - Re) * nnt2;
	
	// In deep step case, choose reflect or refract by russian roulette.
	const double probability = 0.25 + 0.5 * Re;
	if(depth > 2) {
		Ray nxtray;
		if(rnd->next01() < probability) { // reflection
			nxtray = reflection_ray;
			nxtray.weight = Color(Re / probability);
		} else { // refraction
			nxtray = refraction_ray;
			nxtray.weight = Color(Tr / (1.0 - probability));
		}
		outvecs->push_back(nxtray);
	} else { // shallow case, calculate both
		Ray reray;
		reray = reflection_ray;
		reray.weight = Color(Re);
		outvecs->push_back(reray);
		
		Ray trray;
		trray = refraction_ray;
		trray.weight = Color(Tr);
		outvecs->push_back(trray);
	}
}
*/

BSDF::Sample FullRefractionBSDF::getSample(Renderer::Context *cntx, const FinalIntersection &isect) {
    Sample smpl;
    smpl.flag = 0;
    
    const Ray inray = cntx->getCurrentInsidentRay();
    const R1hFPType idotn =Vector3::dot(inray.direction, isect.shadingNormal);
    const Vector3 reflect_dir = inray.direction - isect.shadingNormal * 2.0 * idotn;
    const bool into = idotn < 0.0;
    const Vector3 abs_normal = into? isect.shadingNormal : (isect.shadingNormal * -1.0);
    
    if(into) {
        smpl.flag |= kInTo;
    }
    
    // Snell's law
    const R1hFPType n1n2 = into? (1.0 / ior) : (ior / 1.0);
    const R1hFPType cos2t = 1.0 - n1n2 * n1n2 * (1.0 - idotn * idotn); // power of 2 makes idotn's sign is don't care.
    
    // full reflection part
    if(cos2t < 0.0) {
        smpl.position = isect.position;
        smpl.normal = abs_normal;
        smpl.direction = reflect_dir;
        smpl.bsdf = 1.0;
        smpl.pdf = std::abs(idotn);
        smpl.flag |= kReflected;
        return smpl;
    }
    
    // refraction part
    const Vector3 refract_dir = Vector3::normalized(inray.direction * n1n2 - abs_normal * (-1.0 * std::abs(idotn) * n1n2 + sqrt(cos2t)));
    
    smpl.position = isect.position;
    smpl.normal = abs_normal * -1.0;
    smpl.direction = refract_dir;
    smpl.bsdf = n1n2 * n1n2 / std::abs(Vector3::dot(smpl.normal, smpl.direction));
    smpl.pdf = 1.0;
    //smpl.flag = 0;
    
    return smpl;
}

R1hFPType FullRefractionBSDF::evaluate(const Sample& insident, const Sample& outgoing) {
    // TODO
    // reflection case
    
    // refraction case
    
    return 1.0 / std::abs(Vector3::dot(outgoing.normal, outgoing.direction));
}

R1hFPType FullRefractionBSDF::probabilityForSample(const Sample& insident, const Sample& smpl) {
    // TODO
    // reflection case
    
    // refraction case
    
    return 1.0;
}

void FullRefractionBSDF::setIor(R1hFPType nior) {
	ior = nior;
}

R1hFPType FullRefractionBSDF::getIor() const {
    return ior;
}
