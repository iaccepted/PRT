#include "Scene.h"

void Scene::loadAllData(int sqrtN)
{

	std::vector<string> names = { "sphere.txt", "walla.txt", "wallb.txt", "wallc.txt" };


	std::vector<glm::vec3> color = { glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 1.0), glm::vec3(0.0, 1.0, 1.0), glm::vec3(0.0, 1.0, 1.0) };

	int nFuncs = sqrtN * sqrtN;
	char *path = new char[128];
	int nMesh = names.size();
	for (int i = 0; i < nMesh; ++i)
	{

		Object *obj = new Object();

		objects.push_back(obj);

		int nVer, nIdx;
		FILE *in = fopen(names[i].c_str(), "rb");

		fread(&nVer, sizeof(int), 1, in);
		obj->vertices.resize(nVer);

		for (int j = 0; j < nVer; ++j)
		{
			obj->vertices[j].unshadowedCoeffs = new double[nFuncs];
			obj->vertices[j].shadowedCoeffs = new double[nFuncs];


			fread(&obj->vertices[j].position, sizeof(glm::vec3), 1, in);
			fread(&obj->vertices[j].normal, sizeof(glm::vec3), 1, in);
			fread(obj->vertices[j].unshadowedCoeffs, sizeof (double), nFuncs, in);
			fread(obj->vertices[j].shadowedCoeffs, sizeof(double), nFuncs, in);

			obj->vertices[j].diffuseMaterial= color[i];
		}

		fread(&nIdx, sizeof(int), 1, in);
		obj->indices.resize(nIdx);
		for (int j = 0; j < nIdx; ++j)
		{
			fread(&obj->indices[j], sizeof(GLuint), 1, in);
		}
		fclose(in);
	}
}

void Scene::saveAllData(int sqrtN)
{
#ifdef STD
	const char *names[6] = { "sphere.txt", "walla.txt", "wallb.txt", "wallc.txt" };
#else
	const char *names[6] = { "head_optimized.txt" };
#endif
	int numFunctions = sqrtN * sqrtN;
	int nObj = objects.size();

	for (int i = 0; i < nObj; ++i)
	{
		FILE *out = fopen(names[i], "wb");

		std::vector<Vertex> &vertices = objects[i]->vertices;
		int nVer = vertices.size();

		fwrite(&nVer, sizeof(int), 1, out);
		for (int j = 0; j < nVer; ++j)
		{
			fwrite(&vertices[j].position, sizeof(glm::vec3), 1, out);
			fwrite(&vertices[j].normal, sizeof(glm::vec3), 1, out);
			fwrite(vertices[j].unshadowedCoeffs, sizeof(double), numFunctions, out);
			fwrite(vertices[j].shadowedCoeffs, sizeof(double), numFunctions, out);
		}

		std::vector<GLuint> &indices = objects[i]->indices;
		int nIdx = indices.size();
		fwrite(&nIdx, sizeof(int), 1, out);
		for (int j = 0; j < nIdx; ++j)
		{
			fwrite(&indices[j], sizeof(GLuint), 1, out);
		}

		fflush(out);
		fclose(out);
	}
}

bool Scene::addModelFromFile(const char* path)
{
	Object* obj = new Object();
	obj->load(path);
	unsigned index = objects.size() + 1;

	cout << "Model " << index << ": vertices = " << obj->vertices.size() << ", triangles = " << obj->indices.size() / 3 << endl;
	numAllVertices += obj->vertices.size();
	numAllIndices += obj->indices.size();
	objects.push_back(obj);
	return true;
}

void Scene::printFirst(const int idx, int n)
{
	Object *obj = objects[idx];

	int vn = obj->vertices.size();
	if (vn < n)n = vn;

	std::vector<Vertex> &vertices = obj->vertices;
	std::vector<GLuint> &ivec = obj->indices;

	int nFunctions = BAND * BAND;
	for (int i = 0; i < n; ++i)
	{
		int ii = ivec[i];
		/*glm::vec3 &p = vertices[ii].position;
		glm::vec3 &nor = vertices[ii].normal;
		printf("%.5f %.5f %.5f--%.5f %.5f %.5f\n", p.x, p.y, p.z, nor.x, nor.y, nor.z);*/

		const double *tlm = vertices[ii].shadowedCoeffs;

		for (int j = 0; j < nFunctions; ++j)
		{
			printf("%.5f ", tlm[j]);
		}
	}
}

bool Scene::generateCoeffs(Sampler &sampler, int numBands)
{
	if (!initCoeffs(numBands))return false;

	
	this->generateDirectCoeffs(sampler, numBands);

	return true;
}

bool Scene::initCoeffs(int numBands)
{
	const int numFunctions = numBands * numBands;

	const unsigned nObjs = objects.size();
	for (unsigned objIdx = 0; objIdx < nObjs; ++objIdx)
	{
		Object *curObject = objects[objIdx];
		const unsigned nVertices = curObject->vertices.size();
		
		for (unsigned verIdx = 0; verIdx < nVertices; ++verIdx)
		{
			Vertex &curVertex = curObject->vertices[verIdx];

			curVertex.unshadowedCoeffs = new double[numFunctions];

			curVertex.shadowedCoeffs = new double[numFunctions];

			if (NULL == curVertex.unshadowedCoeffs || NULL == curVertex.shadowedCoeffs)
			{
				cerr << "Failed to allocate memory for coefficients." << endl;
				return false;
			}

			for (int i = 0; i < numFunctions; ++i)
			{
				curVertex.unshadowedCoeffs[i] = 0.0f;
				curVertex.shadowedCoeffs[i] = 0.0f;
			}
		}
	}
	return true;
}

void Scene::generateDirectCoeffs(Sampler &sampler, int nBands)
{
	const unsigned nFunctions = nBands * nBands;
	const unsigned nSamples = sampler.size();

	cout << "Compute unshadowed and shadowed SH." << endl;
	unsigned interval = 0, curNumVertices = 0;

	unsigned nObjs = objects.size();
	for (unsigned objIdx = 0; objIdx < nObjs; ++objIdx)
	{
		Object *curObject = objects[objIdx];
		unsigned nVertices = curObject->vertices.size();

		for (unsigned verIdx = 0; verIdx < nVertices; ++verIdx, ++curNumVertices)
		{
			
			if (curNumVertices == interval)
			{
				cout << curNumVertices * 100 / numAllVertices << "% ";
				interval += numAllVertices / 10;
			}
			Vertex &curVertex = objects[objIdx]->vertices[verIdx];

			std::vector<SAMPLE> &samples = sampler.samples;
			for (unsigned sampleIdx = 0; sampleIdx < nSamples; ++sampleIdx)
			{
				double cosine = glm::dot(samples[sampleIdx].xyz, curVertex.normal);

				if (cosine > 0.0f)
				{
					Ray ray(curVertex.position + 2 * EPSILON * curVertex.normal,
						samples[sampleIdx].xyz);

					//unsigned hitObjectIdx, hitTriangleIdx;
					bool blocked = isRayBlocked(ray);

					for (unsigned l = 0; l < nFunctions; ++l)
					{
						double contribution = cosine * samples[sampleIdx].shValues[l];
						curVertex.unshadowedCoeffs[l] += contribution;

						if (!blocked)
						{
							curVertex.shadowedCoeffs[l] += contribution;
						}
					}
				}
			}

			//rescale the coefficients
			double scale = 4 * M_PI / nSamples;
			for (unsigned l = 0; l < nFunctions; ++l)
			{
				curVertex.unshadowedCoeffs[l] *= scale;
				curVertex.shadowedCoeffs[l] *= scale;
			}
		}
	}
}

bool Scene::isRayBlocked(Ray& ray)
{
	for (unsigned i = 0; i < objects.size(); ++i) {
		bool ret = objects[i]->doesRayHitObject(ray);
		if (ret)return true;
	}
	return false;
}