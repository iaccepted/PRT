/***************************************
*author: guohongzhi  zju
*date:2015.8.26
*func: light
****************************************/

#ifndef LIGHT_H
#define LIGHT_H

#include "Sampler.h"
#include "SHRotation.h"
#include "FreeImage.h"

struct Image {
	Image() : width(0), height(0){
		pixel[0] = NULL;
		pixel[1] = NULL;
		pixel[2] = NULL;
	}
	~Image() {
		for (int i = 0; i < 3; i++)
		if (!pixel[i]) { delete[]pixel[i]; pixel[i] = NULL; }
	}

	int width;
	int height;
	float* pixel[3];
	bool loadFromFile(const char* filename);
};

class Light
{
public:
	Light(const unsigned nBands);

	//the result store in the lightCoeffs
	void directLight(const Sampler &sampler);
	void lightFromImage(const char* imagePath, Sampler& sampler, int numBands, float lightIntensity = 0.4f);
	void rotateLightCoeffs(const double theta, const double phi);

	//SH coefficients for light source, before and after rotation
	double *unrotatedCoeffs[3];
	double *rotatedCoeffs[3];
	unsigned nBands;

private:
	double lightIntensity(double theta, double phi);
};



#endif