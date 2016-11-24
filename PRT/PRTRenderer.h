/***************************************
*author: guohongzhi  zju
*date:2015.8.22
*func: manage render
****************************************/
#ifndef _PRTRENDERER_H_
#define _PRTRENDERER_H_

#include "Global.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Light.h"
#include "ProgramManager.h"

typedef enum LIGHTING_TYPE
{
	LIGHTING_TYPE_SH_UNSHADOWED = 0,
	LIGHTING_TYPE_SH_SHADOWED,
	LIGHTING_TYPE_SH_SHADOWED_BOUNCE_1,
	LIGHTING_TYPE_SH_SHADOWED_BOUNCE_2,
	LIGHTING_TYPE_SH_SHADOWED_BOUNCE_3
}LIGHT_TYPE;


class PRTRenderer
{
public:
	PRTRenderer(int _width = 480, int _height = 480);
	void renderSceneWithLight(Scene &scene, Light &light);
	void compileShaderFromFile(const char *verFileName, const char *fragFileName);
	void setUniform();
	void precomputeColor(Scene &scene, Light &light);
	~PRTRenderer(void);

private:
	int width;
	int height;
	mat4 modelMatrix;
	mat4 viewMatrix;

	ProgramManager prog;

	GLFWwindow* window;

	void initGLFW();
	void initGL();

	// GLFW callback
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void cursorPositionCallback(GLFWwindow* window, double x, double y);
	static void scrollCallback(GLFWwindow* window, double x, double y);
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	static LIGHTING_TYPE lightType;
	static double phi;
	static double theta;
	static double preMouseX;
	static double preMouseY;
	static bool isLeftButtonPress;
	static bool isNeedRotate;
};

#endif