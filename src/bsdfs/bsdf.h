#ifndef R1H_ABSTRACT_BSDF_H
#define R1H_ABSTRACT_BSDF_H

#include <vector>
#include <memory>

#include <r1htypes.h>
#include <ray.h>
#include <intersection.h>
#include <renderer.h>
#include <random.h>

namespace r1h {

/// abstract class
class BSDF {
public:
    struct Sample {
        Vector3 position;
        Vector3 direction;
        Vector3 normal;     // this is used for small offset.
        R1hFPType bsdf;
        R1hFPType pdf;
        int flag;
        
        Vector3 smallOffsetedPosition() const;
        Ray getRay() const;
    };
    
    enum BSDFType {
        kDiffuse    = 1,
        kSpecular   = 2,
        kGlossy     = 4,
        kRefraction = 8,
        kTransmit   = 16
    };
    
public:
    // correct  incorrect
    // vi->|    vi->|
    //   <-|ng    <-|ng
    // vo<-|        |->vo
    static bool isCurrectBRDF(const Vector3& vi, const Vector3& vo, const Vector3& ng);
    
public:
    virtual ~BSDF() {}
    
    virtual int getType() const = 0;
    
    virtual Sample getSample(Renderer::Context *cntx, const FinalIntersection &isect) = 0;
    virtual R1hFPType evaluate(const Sample& insident, const Sample& outgoing) = 0;
    virtual R1hFPType probabilityForSample(const Sample& insident, const Sample& smpl) = 0;
};

typedef std::shared_ptr<BSDF> BSDFRef;

}
#endif
