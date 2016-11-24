#include "AABB.h"

AABB::AABB() :maxp(vec3(FLT_MIN, FLT_MIN, FLT_MIN)), minp(vec3(FLT_MAX, FLT_MAX, FLT_MAX)){}

void AABB::expand(const vec3 &v)
{
	if (v.x > maxp.x)maxp.x = v.x;
	if (v.y > maxp.y)maxp.y = v.y;
	if (v.z > maxp.z)maxp.z = v.z;

	if (v.x < minp.x)minp.x = v.x;
	if (v.y < minp.y)minp.y = v.y;
	if (v.z < minp.z)minp.z = v.z;
}

MAJOR_AXIS AABB::majorAxis()
{
	vec3 distance = maxp - minp;
	if (distance.x > distance.y && distance.x > distance.z)return AXIS_X;
	else if (distance.y > distance.z)return AXIS_Y;
	else return AXIS_Z;
}

bool AABB::isPointInside(const vec3 &point) const
{
	bool ret = ((point.x < maxp.x) &&
		(point.y <= maxp.y) &&
		(point.z <= maxp.z) &&
		(point.x >= minp.x) &&
		(point.y >= minp.y) &&
		(point.z >= minp.z));
	return ret;
}

//Slabs method to check intersecting problem
bool AABB::isIntersect(Ray &ray, double *hitt0, double *hitt1) const
{
	/*float t0 = FLT_MIN, t1 = FLT_MAX;
	for (int i = 0; i < 3; ++i) {
		float invRayDir = 1.f / ray.direction[i];
		float tNear = (minp[i] - ray.orig[i]) * invRayDir;
		float tFar = (maxp[i] - ray.orig[i]) * invRayDir;

		if (tNear > tFar) std::swap(tNear, tFar);
		t0 = tNear > t0 ? tNear : t0;
		t1 = tFar  < t1 ? tFar : t1;
		if (t0 > t1) return false;
	}
	if (hitt0) *hitt0 = t0;
	if (hitt1) *hitt1 = t1;
	return true;*/

	vec3 &source = ray.orig;
	vec3 &direction = ray.direction;

	if (isPointInside(source))
	{
		if (hitt0)
			*hitt0 = 0.0f;

		return true;
	}

	//Keep track of the largest tNear and smallest tFar
	 //float tNear = -99999.0f, tFar = 99999.0f;
	 float tNear = FLT_MIN, tFar = FLT_MAX;
	//If the ray is parallel to the x planes
	if (direction.x == 0.0f)
	{
		//If the ray origin is not between the x extents of the box, return false
		if (source.x<minp.x || source.x>maxp.x)
			return false;
	}
	else
	{
		//The ray is not parallel to the planes
		//Compute the intersection distances
		float t1 = (minp.x - source.x) / direction.x;
		float t2 = (maxp.x - source.x) / direction.x;

		//Make sure t1 is the distance to the intersection with the near plane
		if (t1>t2)
		{
			float temp = t1;
			t1 = t2;
			t2 = temp;
		}

		//Update tNear and tFar
		if (t1>tNear)
			tNear = t1;

		if (t2<tFar)
			tFar = t2;
	}

	//Repeat for Y and Z planes
	if (direction.y == 0.0f)
	{
		if (source.y<minp.y || source.y>maxp.y)
			return false;
	}
	else
	{
		float t1 = (minp.y - source.y) / direction.y;
		float t2 = (maxp.y - source.y) / direction.y;

		if (t1>t2)
		{
			float temp = t1;
			t1 = t2;
			t2 = temp;
		}

		if (t1>tNear)
			tNear = t1;

		if (t2<tFar)
			tFar = t2;
	}

	if (direction.z == 0.0f)
	{
		if (source.z<minp.z || source.z>maxp.z)
			return false;
	}
	else
	{
		float t1 = (minp.z - source.z) / direction.z;
		float t2 = (maxp.z - source.z) / direction.z;

		if (t1>t2)
		{
			float temp = t1;
			t1 = t2;
			t2 = temp;
		}

		if (t1>tNear)
			tNear = t1;

		if (t2<tFar)
			tFar = t2;
	}

	//If tNear>tFar, no intersection
	if (tNear>tFar)
		return false;

	//If tFar<0, box is behind ray
	if (tFar<0)
		return false;

	//Save entryT if required
	if (hitt0)
	{
		*hitt0 = tNear;
	}
	if (hitt1)
	{
		*hitt1 = tFar;
	}

	return true;
}