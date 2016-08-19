#ifndef R1H_ABSTRACT_TEXTURE_H
#define R1H_ABSTRACT_TEXTURE_H

#include <vector>
#include <memory>
#include <r1htypes.h>
#include <asset.h>
#include <intersection.h>

namespace r1h {

class SceneObject;
class Intersection;
	
/// base class
class Texture : public Asset {
public:
	enum MapType {
		kUV,
		kWorld,
		kLocal
    };
    // almostly for normal map
    enum MapSpace {
        kWorldSpace,
        kObjectSpace,
        kTangentSpace
    };
	
public:
	Texture();
	
	// must override
	virtual ~Texture();
    virtual Color sample(const Vector3 &p) const = 0;
	
	// option
	virtual Color sample(const FinalIntersection *isect) const;
    
    virtual Vector3 sampleAsVector(const Vector3 &p) const;
    virtual Vector3 sampleAsVector(const FinalIntersection *isect) const;
    
	virtual Vector3 applyTransform(const Vector3 &p) const;
	virtual void setIsUseTransform(const bool isuse);
	virtual void setTransform(const Matrix4 m);
	
	virtual void setMapType(int type);
    virtual int getMapType() const;
    virtual void setMapSpace(int spc);
    virtual int getMapSpace() const;
    
private:
	bool isUseTransform;
	Matrix4 transform;
	
    int mapType;
    int mapSpace;
};

typedef std::shared_ptr<Texture> TextureRef;

/// stub
class StubTexture : public Texture {
public:
	StubTexture(const std::string& refid) : Texture() {
		assetId = refid;
		assetType = Asset::Type::kStub;
	};
	~StubTexture() {};
	
	Color sample(const Vector3 &p) const { return Color(1.0, 0.0, 0.0); };
};

/// Constant color
class ConstantColorTexture : public Texture {
public:
	ConstantColorTexture(const Color col=Color(1.0,0.0,0.0));
	~ConstantColorTexture();
	
	Color sample(const Vector3 &p) const;
	void setColor(const Color& col);
	
private:
	Color color;
};

}
#endif
