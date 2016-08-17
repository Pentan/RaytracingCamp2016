#ifndef R1H_SCENE_H
#define R1H_SCENE_H

#include <vector>
#include "r1htypes.h"
#include "ray.h"
#include "renderer.h"
#include "sceneobject.h"
#include "materials/material_include.h"
#include "camera.h"
#include "intersection.h"
#include "bvhnode.h"

namespace r1h {

class Camera;

class Scene : public BVHLeaf {
public:
    Scene();
    ~Scene();
    
    // setup
    int addObject(SceneObjectRef objref);
    SceneObject* getObject(int objid) const;
	int getObjectsCount() const;
	
    Camera* getCamera();
	void setCamera(CameraRef camref);
    
	SkyMaterial* getSkyMaterial() const;
	void setSkyMaterial(SkyMaterialRef matref);
    
	void prepareRendering();
	
    bool isIntersect(const Ray &ray, Intersection *intersect) const;
    
	// render
    //Color radiance(Renderer::Context *cntx, const Ray &ray);
    
	// override BVHLeaf
	bool isIntersectLeaf(int dataid, const Ray &ray, Intersection *intersect) const;
	
private:
    CameraRef camera;
    std::vector<SceneObjectRef> sceneObjects;
    BVHNode *objectBVH;
    
    SkyMaterialRef skyMaterial;
	
    // -> isIntersect
	//bool intersectSceneObjects(const Ray &ray, Intersection *intersect);
};

}

#endif
