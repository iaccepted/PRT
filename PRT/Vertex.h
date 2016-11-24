#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "Global.h"

class Vertex
{
public:
	glm::vec3 position;
	glm::vec3 normal;

	//Color of vertex including lighting
	glm::vec3 litColor;

	//Diffuse material color
	glm::vec3 diffuseMaterial;

	//SH coefficients for transfer function
	double *unshadowedCoeffs;
	double *shadowedCoeffs;


	Vertex() : litColor(1.f, 1.f, 1.f), unshadowedCoeffs(NULL), diffuseMaterial(1.f, 1.f, 1.f)
	{
		shadowedCoeffs = NULL;
	}

	Vertex(float px, float py, float pz, float nx, float ny, float nz)
		: position(px, py, pz), normal(nx, ny, nz), litColor(1.f, 1.f, 1.f), unshadowedCoeffs(NULL),
		diffuseMaterial(1.f, 1.f, 1.f)
	{
		normal = glm::normalize(normal);
		
		shadowedCoeffs = NULL;
			
	}

	~Vertex() {
		delete[] unshadowedCoeffs;
		unshadowedCoeffs = NULL;

		delete[] shadowedCoeffs;
		shadowedCoeffs = NULL;
	}
};

#endif