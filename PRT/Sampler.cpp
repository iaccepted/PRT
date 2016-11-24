#include "Sampler.h"
#include "SHEval.h"

void Sampler::generateSamples(const int sqrtNumSamples, const int numBans)
{
	int numSamples = sqrtNumSamples * sqrtNumSamples;
	int numFunctions = numBans * numBans;
	SHEvalFunc SHEval[] = { SHEval3, SHEval3, SHEval3, SHEval3, SHEval4, SHEval5, SHEval6, SHEval7, SHEval8, SHEval9, SHEval10 };

	samples.resize(numSamples); 

	//srand(unsigned(time(NULL)));

	unsigned index = 0;
	for (int i = 0; i < sqrtNumSamples; ++i)
	{
		for (int j = 0; j < sqrtNumSamples; ++j)
		{
			double a = (i + ((double)rand() / RAND_MAX)) / (double)sqrtNumSamples;
			double b = (j + ((double)rand() / RAND_MAX)) / (double)sqrtNumSamples;

			double theta = 2.0 * acos(sqrt(1.0 - a));
			double phi = 2.0 * M_PI * b;
			double x = sin(theta) * cos(phi);
			double y = sin(theta) * sin(phi);
			double z = cos(theta);
			samples[index].xyz = glm::vec3(x, y, z);
			samples[index].phi = phi;
			samples[index].theta = theta;

			samples[index].shValues = new double[numFunctions];
			if (!samples[index].shValues)
			{
				cerr << "Unable to allocate space for SH values in samples" << endl;
				exit(-1);
			}
			SHEval[numBans](x, y, z, samples[index].shValues);

			++index;
		}
	}

	/*for (int i = 200; i < 205; ++i)
	{
		printf("%0.5f-%0.5f-%0.5f ** %0.5f-%0.5f-%0.5f\n", samples[i].xyz.x, samples[i].xyz.y, samples[i].xyz.z, samples[i].shValues[0], samples[i].shValues[1], samples[i].shValues[2]);
	}*/
}

unsigned Sampler::size()
{
	return this->samples.size();
}

SAMPLE &Sampler::operator[](unsigned int index)
{
	assert(index >= 0 && index < samples.size());
	return samples[index];
}