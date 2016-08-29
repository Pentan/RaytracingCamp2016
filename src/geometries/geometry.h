#ifndef R1H_ABSTRACT_GEOMETRY_H
#define R1H_ABSTRACT_GEOMETRY_H

#include <memory>

#include <r1htypes.h>
#include <random.h>
#include <ray.h>
#include <aabb.h>
#include <intersection.h>
#include <asset.h>

namespace r1h {
	
class Geometry : public Asset {
public:
    struct SamplePoint {
        Vector3 position;
        Vector3 normal;
        R1hFPType pdf;
    };
    
public:
    Geometry() {}
    virtual ~Geometry() {};
    
    virtual AABB getAABB(const Matrix4& tm) const = 0;
    virtual bool isIntersect(const Ray &ray, Intersection *intersect) const = 0;
	
    virtual SamplePoint getSamplePoint(Random *rng) const = 0;
    
	// optional
	virtual void prepareRendering() { /* ex.:build VBH for mesh */ }
};

typedef std::shared_ptr<Geometry> GeometryRef;

/// stub
class StubGeometry : public Geometry {
public:
	StubGeometry(const std::string& refid) : Geometry() {
		assetId = refid;
		assetType = Asset::Type::kStub;
	}
	~StubGeometry() {}
	
	virtual AABB getAABB(const Matrix4& tm) const { return AABB(0.0, 0.0); };
	virtual bool isIntersect(const Ray &ray, Intersection *intersect) const { return false; };
};

}
#endif
