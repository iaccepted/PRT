/***************************************
*author: guohongzhi  zju
*date:2015.8.24
*func: axis-aligned bounding box
****************************************/

#ifndef _AABB_H_
#define _AABB_H_

#include "Global.h"
#include "Ray.h"

typedef enum MAJOR_AXIS
{
	AXIS_X = 0, AXIS_Y, AXIS_Z
}MAJOR_AXIS;

class AABB
{
public:
	AABB();
	//judge if the point in the aabb
	bool isPointInside(const vec3 &point) const;

	//test that if the ray intersect the aabb
	//hitt0 store the near distance and the hitt1 store the far distance
	bool isIntersect(Ray &ray, double *hitt0 = NULL, double *hitt1 = NULL) const;
	MAJOR_AXIS majorAxis();
	void expand(const vec3 &v);
	vec3 maxp;
	vec3 minp;
};

#endif