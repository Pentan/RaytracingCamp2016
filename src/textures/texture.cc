#include <iostream>

#include "texture.h"
#include <random.h>
#include <intersection.h>
#include <geometries/geometry_include.h>
#include <sceneobject.h>

using namespace r1h;

/// base class

Texture::Texture():
	mapType(kWorld),
    mapSpace(kTangentSpace),
	isUseTransform(true)
{}

Texture::~Texture() {}

//virtual Color sample(const Vector3 &p) const = 0;
Color Texture::sample(const FinalIntersection *isect) const {
	Vector3 p(0.0);
    const SceneObject *obj = isect->objectRef;
	
	switch (mapType) {
		case kUV:
		{
			// TODO: UV is vertex attribute 0.
            const Geometry *geom = obj->getGeometry();
			if(geom->getAssetType() == Mesh::kTypeID) {
				const Mesh *mesh = reinterpret_cast<const Mesh*>(geom);
				p = mesh->getVaryingAttr(isect->faceId, 0, isect->varyingWeight);
			}
		}
			break;
		case kWorld:
			p = isect->position;
			break;
		case kLocal:
			p = obj->toLocalPosition(isect->position);
            break;
        default:
            fprintf(stderr, "unknown map type\n");
            abort();
	}
	
	applyTransform(p);
	return sample(p);
}

Vector3 Texture::sampleAsVector(const Vector3 &p) const {
    Color c = sample(p) * 2.0 - 1.0;
    return Vector3(c.r, c.g, c.b);
}

Vector3 Texture::sampleAsVector(const FinalIntersection *isect) const {
    Color c = sample(isect) * 2.0 - 1.0;
    Vector3 v(c.r, c.g, c.b);
    
    switch(mapSpace) {
        case kTangentSpace:
            v = Matrix4::mulV3(isect->tangentSpaceBasis, v);
            break;
        case kWorldSpace:
            // noop
            break;
        case kObjectSpace:
            isect->objectRef->toWorldNormal(v);
            break;
        default:
            fprintf(stderr, "unknown map space\n");
            abort();
    }
    v.normalize();
    
    return v;
}

void Texture::setIsUseTransform(const bool isuse) {
 isUseTransform = isuse;
}
void Texture::setTransform(const Matrix4 m){
	transform = m;
}

Vector3 Texture::applyTransform(const Vector3 &p) const {
    if(!isUseTransform) {
        return p;
    }
    return Matrix4::transformV3(transform, p);
}

void Texture::setMapType(int type) {
	mapType = type;
}
int Texture::getMapType() const {
	return mapType;
}

void Texture::setMapSpace(int spc) {
    mapSpace = spc;
}
int Texture::getMapSpace() const {
    return mapSpace;
}

/// constant color
ConstantColorTexture::ConstantColorTexture(const Color col): color(col) {
}

ConstantColorTexture::~ConstantColorTexture()
{}

Color ConstantColorTexture::sample(const Vector3 &p) const {
	return color;
}

void ConstantColorTexture::setColor(const Color& col) {
	color = col;
}
