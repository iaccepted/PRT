#ifndef _SCENE_H_
#define _SCENE_H_
#include "Global.h"
#include "Ray.h"
#include "Object.h"
#include "Sampler.h"

class Scene
{
public:
	Scene() :numAllVertices(0), numAllIndices(0){}
	bool addModelFromFile(const char* path);
	bool generateCoeffs(Sampler &sampler, int numBands);
	//void bindBuffer();
	void printFirst(const int idx, int n);//test, print the idxth object's data, 0 - n index - vertex information

	void loadAllData(int nBands);
	void saveAllData(int nBands);

	bool isRayBlocked(Ray& ray);
	std::vector<Object*> objects;

	GLuint VB;
	GLuint IB;
	unsigned long numAllIndices;

	unsigned long numAllVertices;

private:
	bool initCoeffs(int numBands);
	void generateDirectCoeffs(Sampler &sampler, int numBands);
};

#endif