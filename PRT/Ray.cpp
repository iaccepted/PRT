#include "Ray.h"

Ray::Ray() :tmin(0.0f), tmax(FLT_MAX){}
Ray::Ray(vec3 newOrig, vec3 newDir) : tmin(0.0f), tmax(FLT_MAX), orig(newOrig), direction(newDir)
{
	direction = glm::normalize(direction);
}

void Ray::set(vec3 newOrig, vec3 newDir)
{
	orig = newOrig;
	direction = glm::normalize(newDir);
}

//this function is modified by a function in directx (windows graphics api)
bool Ray::intersectTriangle(const vec3 &v0, const vec3 &v1, const vec3 &v2)
{
	vec3 E1 = v1 - v0;
	vec3 E2 = v2 - v0;
	vec3 P = glm::cross(direction, E2);

	//determinant
	double det = glm::dot(E1, P);

	//keep det > 0, modify T accordingly
	vec3 T;
	if (det > 0)
	{
		T = orig - v0;
	}
	else
	{
		T = v0 - orig;
		det = -det;
	}

	//if determinant is near zero, ray lies in plane of triangle
	if (det < 0.00001f)return false;

	//calculate u and make sure u <= 1
	double u = glm::dot(T, P);
	if (u < 0.0f || u > det)return false;

	vec3 Q = glm::cross(T, E1);

	//Calculate v and make sure u + v <= 1
	double v = glm::dot(direction, Q);
	if (v < 0.0f || u + v > det)return false;

	//calculate t ,scale parameters, ray intersects triangle
	double t = glm::dot(E2, Q);
	double fInvDet = 1.0f / det;
	t *= fInvDet;

	if (t < tmin || t > tmax)return false;
	return true;
}