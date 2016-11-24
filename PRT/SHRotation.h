/***************************************
*author: guohongzhi  zju
*date:2015.8.26
*func: rotate sh
****************************************/
#ifndef _SHROTATION_H_
#define _SHROTATION_H_
#include "Global.h"

void rotateSHCoefficients(int numBands, double * unrotatedCoeffs, double * rotatedCoeffs, float theta, float phi);
void getZRotationMatrix(int band, double * entries, double angle);
void getX90DegreeRotationMatrix(int band, double * entries);
void applyMatrix(int size, double * matrix, bool transpose,
	double * inVector, double * outVector);

#endif