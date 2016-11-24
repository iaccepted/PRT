/***************************************
*author: guohongzhi zju
*date:2015.8.22
*func: define a ray and offer function to check
 if a ray intersect a triangle
****************************************/

#ifndef _RAY_H_
#define _RAY_H_
#include "Global.h"

class Ray
{
public:
	vec3 orig;
	vec3 direction;
	double tmin, tmax;

	Ray();
	Ray(vec3 newOrig, vec3 newDir);
	void set(vec3 newOrig, vec3 newDir);
	bool intersectTriangle(const vec3 &v0, const vec3 &v1, const vec3 &v2);
	vec3 operator()(float t) const { return orig + direction*t; }
	
};
#endif