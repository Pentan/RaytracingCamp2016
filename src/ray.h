#ifndef R1H_RAY_H
#define R1H_RAY_H

#include "r1htypes.h"

namespace r1h {

struct Ray {
    Vector3 origin;
    Vector3 direction;
    Color weight;
    int fromBSDF;
    
    Ray() :
        origin(0.0, 0.0, 0.0),
		direction(0.0, 0.0, -1.0),
		weight(1.0),
        fromBSDF(0)
    {};
    
    Ray(const Vector3 &org, const Vector3 &dir, const int bsdft=-1):
		origin(org),
		direction(dir),
        weight(1.0),
        fromBSDF(bsdft)
    {};
    
    Ray(const Vector3 &org, const Vector3 &dir, const Color &col, const int bsdft=-1):
		origin(org),
		direction(dir),
        weight(col),
        fromBSDF(bsdft)
    {};
    
	Ray makeTransformed(Matrix4 m) const {
		Ray ret = *this;
		ret.origin = Matrix4::transformV3(m, origin);
		ret.direction = Matrix4::mulV3(m, direction);
		ret.direction.normalize();
		return ret;
	}
};

}
#endif
