#include "Light.h"

Light::Light(const unsigned nBands)
{
	this->nBands = nBands;

	unsigned nFunctions = nBands * nBands;
	for (unsigned i = 0; i < 3; ++i)
	{
		unrotatedCoeffs[i] = new double[nFunctions];
		rotatedCoeffs[i] = new double[nFunctions];
	}
}

double Light::lightIntensity(double theta, double phi)
{
	return theta < (M_PI / 6) ? 1 : 0;
}

void Light::directLight(const Sampler &sampler)
{
	unsigned nFunctions = nBands * nBands;
	unsigned nSamples = sampler.samples.size();
	const std::vector<SAMPLE> &samples = sampler.samples;

	for (unsigned i = 0; i < nFunctions; ++i)
	{
		unrotatedCoeffs[0][i] = 0.0f;
		unrotatedCoeffs[1][i] = 0.0f;
		unrotatedCoeffs[2][i] = 0.0f;

		for (unsigned j = 0; j < nSamples; ++j)
		{
			unrotatedCoeffs[0][i] += lightIntensity(samples[j].theta, samples[j].phi) * samples[j].shValues[i];
			unrotatedCoeffs[1][i] += lightIntensity(samples[j].theta, samples[j].phi) * samples[j].shValues[i];
			unrotatedCoeffs[2][i] += lightIntensity(samples[j].theta, samples[j].phi) * samples[j].shValues[i];
		}

		unrotatedCoeffs[0][i] *= 4 * M_PI / nSamples;
		unrotatedCoeffs[1][i] *= 4 * M_PI / nSamples;
		unrotatedCoeffs[2][i] *= 4 * M_PI / nSamples;
	}

	/*for (int i = 0; i < nFunctions; ++i)
	{
		printf("%0.5f\n", unrotatedCoeffs[0][i]);
	}*/
}

void LightProbeAccess(vec3* color, Image* image, vec3& direction)
{
	float d = sqrt(direction.x*direction.x + direction.y*direction.y);
	float r = (d == 0) ? 0.0f : (1.0f / M_PI / 2.0f) * acos(direction.z) / d;
	float tex_coord[2];
	tex_coord[0] = 0.5f + direction.x * r;
	tex_coord[1] = 0.5f + direction.y * r;
	int pixel_coord[2];
	pixel_coord[0] = int(tex_coord[0] * image->width);
	pixel_coord[1] = int(tex_coord[1] * image->height);
	int pixel_index = pixel_coord[1] * image->width + pixel_coord[0];
	color->r = image->pixel[0][pixel_index];
	color->g = image->pixel[1][pixel_index];
	color->b = image->pixel[2][pixel_index];
}

void Light::lightFromImage(const char* imagePath, Sampler& sampler, int numBands, float lightIntensity)
{
	Image img;
	img.loadFromFile(imagePath);

	if (numBands < 3) {
		numBands = 3;
		cout << "[WARNING] bands must be >=3 and <= 10. Set bands = 3.\n";
	}
	else if (numBands > 10) {
		numBands = 10;
		cout << "[WARNING] bands must be >=3 and <= 10. Set bands = 10\n";
	}

	for (int i = 0; i < numBands*numBands; i++) {
		unrotatedCoeffs[0][i] = 0.0f;
		unrotatedCoeffs[1][i] = 0.0f;
		unrotatedCoeffs[2][i] = 0.0f;
	}


	for (unsigned i = 0; i < sampler.size(); ++i) {
		vec3& direction = sampler.samples[i].xyz;
		vec3 color;
		LightProbeAccess(&color, &img, direction);
		for (int j = 0; j < numBands*numBands; ++j) {
			
			double shFunction = sampler.samples[i].shValues[j];
			unrotatedCoeffs[0][j] += (color.r * shFunction);
			unrotatedCoeffs[1][j] += (color.g * shFunction);
			unrotatedCoeffs[2][j] += (color.b * shFunction);
		}
	}
	double weight = 4.0f * M_PI * lightIntensity;
	double scale = weight / sampler.size();
	for (int i = 0; i < numBands*numBands; i++) {
		unrotatedCoeffs[0][i] *= scale;
		unrotatedCoeffs[1][i] *= scale;
		unrotatedCoeffs[2][i] *= scale;
	}
}


void Light::rotateLightCoeffs(const double theta, const double phi)
{
	rotateSHCoefficients(nBands, unrotatedCoeffs[0], rotatedCoeffs[0], theta, phi);
	rotateSHCoefficients(nBands, unrotatedCoeffs[1], rotatedCoeffs[1], theta, phi);
	rotateSHCoefficients(nBands, unrotatedCoeffs[2], rotatedCoeffs[2], theta, phi);

	/*for (int i = 0; i < 9; ++i)
	{
		printf("%.5f\n", rotatedCoeffs[0][i]);
	}*/
}



bool Image::loadFromFile(const char* filename)
{
	FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(filename, 0);
	FIBITMAP* env = FreeImage_Load(fifmt, filename);
	if (!env) {
		printf("[Error] load image %s failed.\n", filename);
		return false;
	}
	FREE_IMAGE_TYPE image_type = FreeImage_GetImageType(env);

	//env = FreeImage_ConvertTo24Bits(env);
	//env = FreeImage_ConvertToFloat(env);
	FIRGBF* pixels = (FIRGBF*)FreeImage_GetBits(env);
	height = FreeImage_GetWidth(env);
	width = FreeImage_GetHeight(env);

	pixel[0] = new float[height * width];
	pixel[1] = new float[height * width];
	pixel[2] = new float[height * width];

	float factor = 0.6f;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int idx = i*width + j;
			pixel[0][idx] = pixels[idx].red * factor;
			pixel[1][idx] = pixels[idx].green * factor;
			pixel[2][idx] = pixels[idx].blue * factor;
		}
	}
	return true;
}