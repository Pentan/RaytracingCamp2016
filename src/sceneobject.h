#ifndef R1H_SCENEOBJECT_H
#define R1H_SCENEOBJECT_H

#include <string>
#include <vector>
#include <memory>
#include "materials/material_include.h"
#include "geometries/geometry_include.h"
#include "intersection.h"
#include "aabb.h"

namespace r1h {

class SceneObject {
public:
	SceneObject();
	virtual ~SceneObject();
	
	void setGeometry(GeometryRef geom);
	Geometry* getGeometry() const;
	GeometryRef getGeometryRef();
	
	int addMaterial(MaterialRef matref);
	Material* getMaterialById(int matid) const;
	size_t getMaterialCount() const;
	void replaceMaterial(int matid, MaterialRef matref);
	
	void setTransform(const Matrix4& m);
	Matrix4 getTransform() const;
	
	Vector3 toLocalPosition(const Vector3& wp) const;
    Vector3 toWorldNormal(const Vector3& ln) const;
    
	AABB getAABB() const;
	
	void prepareRendering();
	
	bool isIntersect(const Ray &ray, Intersection *intersect);
    Geometry::SamplePoint getSamplePoint(Random *rng) const;
	
	void setName(const std::string &newname);
	std::string getName() const;
    
    bool isLight() const;
    
public:
    int objectId;
    
private:
	GeometryRef geometry;
	std::vector<MaterialRef> materials;
    std::string name;
    size_t lightMaterialCount;

	Matrix4 transform;
	Matrix4 iTransform;
    Matrix4 itTransform;
    
};

typedef std::shared_ptr<SceneObject> SceneObjectRef;

}
#endif
