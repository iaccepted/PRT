/***************************************
*author: guohongzhi zju
*date:2015.8.22
*func: 在2D空间采样，然后将其映射到球面上
*然后计算每个样本点的SH
****************************************/
#ifndef _SAMPLER_H_
#define _SAMPLER_H_

#include "Global.h"

class SAMPLE
{
public:
	glm::vec3 xyz;
	double theta;
	double phi;
	double *shValues;

	SAMPLE() : shValues(NULL){}
	~SAMPLE()
	{
		if (NULL != shValues)
		{
			delete[] shValues;
			shValues = NULL;
		}
	}
};

class Sampler
{
public:
	std::vector<SAMPLE> samples;

	//get the size of samples
	unsigned size();
	//generate samples
	void generateSamples(const int sqrtNumSamples, const int numBans);

	SAMPLE &operator[] (unsigned int index);
};
#endif