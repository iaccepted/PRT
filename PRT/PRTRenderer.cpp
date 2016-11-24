#include "PRTRenderer.h"


LIGHTING_TYPE PRTRenderer::lightType = LIGHTING_TYPE_SH_UNSHADOWED;
double PRTRenderer::theta = 63.8f;
double PRTRenderer::phi = 130.6f;
//double PRTRenderer::theta = 0.0f;
//double PRTRenderer::phi = 0.0f;
double PRTRenderer::preMouseX = 0.0f;
double PRTRenderer::preMouseY = 0.0f;
bool PRTRenderer::isNeedRotate = false;
bool PRTRenderer::isLeftButtonPress = false;

PRTRenderer::PRTRenderer(int _width, int _height) : width(_width), height(_height)
{
	initGLFW();
	initGL();
}

void PRTRenderer::initGL()
{
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	if (err != GLEW_OK)
	{
		cerr << "Failed to init glew : " << glewGetErrorString(err) << endl;
		exit(-1);
	}

	glViewport(0, 0, width, height);
	/* Enable smooth shading */
	glShadeModel(GL_SMOOTH);
	/* Set the background */
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	/* Depth buffer setup */
	glClearDepth(1.0f);
	/* Enables Depth Testing */
	glEnable(GL_DEPTH_TEST);
	/* The Type Of Depth Test To Do */
	glDepthFunc(GL_LEQUAL);
	///* Really Nice Perspective Calculations */
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


	modelMatrix = mat4(1.0f);
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(45.0f), vec3(1.0, 0.0, 0.0));
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(-50.0f), vec3(0.0, 1.0, 0.0));
	
	//viewMatrix = glm::lookAt(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, 0.0f, -10.0f), vec3(0.0f, 1.0f, 0.0f));
	viewMatrix = glm::lookAt(vec3(-4.0f, 6.0f, 4.0f),
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(-90.0f), vec3(1.0, 0.0, 0.0));
}

void PRTRenderer::initGLFW()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(width, height, "PRT", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorPositionCallback);
}

void PRTRenderer::renderSceneWithLight(Scene &scene, Light &light)
{
	light.rotateLightCoeffs(theta, phi);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		//press the left button and drag in the same time
		if (isNeedRotate && isLeftButtonPress)
		{
			//cout << theta << "  " << phi << endl;
			light.rotateLightCoeffs(theta, phi);
			isNeedRotate = false;
		}

		this->precomputeColor(scene, light);

		unsigned nObjects = scene.objects.size();

		GLuint vao, vbo, ebo;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (unsigned i = 0; i < nObjects; ++i)
		{
			Object *curObject = scene.objects[i];
			
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			glGenBuffers(1, &ebo);

			glBindVertexArray(vao);

			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* curObject->vertices.size(), &curObject->vertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* curObject->indices.size(), &curObject->indices[0], GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)0);
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, litColor));
			glEnableVertexAttribArray(1);

			glDrawElements(GL_TRIANGLES, curObject->indices.size(), GL_UNSIGNED_INT, 0);

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glDeleteVertexArrays(1, &vao);
			glDeleteBuffers(1, &vbo);
			glDeleteBuffers(1, &ebo);
		}
		glfwSwapBuffers(window);
	}
	glfwTerminate();
}

void PRTRenderer::compileShaderFromFile(const char *verFileName, const char *fragFileName)
{
	bool ret = prog.compileShaderFromFile(verFileName, GLSLShader::VERTEX);
	if (!ret)
	{
		cerr << "Compile Vertex Shader Error: " << prog.log() << endl;
		exit(-1);
	}
	ret = prog.compileShaderFromFile(fragFileName, GLSLShader::FRAGMENT);
	if (!ret)
	{
		cerr << "Compile Fragment Shader Error: " << prog.log() << endl;
		exit(-1);
	}


	prog.bindAttribLocation(0, "position");
	prog.bindAttribLocation(1, "color");

	ret = prog.link();
	if (!ret)
	{
		cerr << "Link Error: " << prog.log() << endl;
		exit(-1);
	}

	ret = prog.validate();
	if (!ret)
	{
		cerr << "Validate Error: " << prog.log() << endl;
		exit(-1);
	}

	prog.use();
}

void PRTRenderer::setUniform()
{
	glfwGetFramebufferSize(window, &width, &height);
	mat4 projection = glm::perspective(45.0f, float(width) / height, 0.1f, 1000.0f);
	mat4 mv = viewMatrix * modelMatrix;

	prog.setUniform("MVP", projection * mv);
}

void PRTRenderer::precomputeColor(Scene &scene, Light &light)
{
	unsigned nObjects = scene.objects.size();

	if (lightType == LIGHTING_TYPE_SH_UNSHADOWED)
	{
		for (unsigned objIdx = 0; objIdx < nObjects; ++objIdx)
		{
			Object *curObject = scene.objects[objIdx];

			unsigned nVertices = curObject->vertices.size();

			for (unsigned verIdx = 0; verIdx < nVertices; ++verIdx)
			{
				Vertex &curVertex = curObject->vertices[verIdx];

				vec3 brightness = vec3(0.0f, 0.0f, 0.0f);

				unsigned nFunctions = light.nBands * light.nBands;
				for (unsigned l = 0; l < nFunctions; ++l)
				{
					brightness.r += light.rotatedCoeffs[0][l] * curVertex.unshadowedCoeffs[l];
					brightness.g += light.rotatedCoeffs[1][l] * curVertex.unshadowedCoeffs[l];
					brightness.b += light.rotatedCoeffs[2][l] * curVertex.unshadowedCoeffs[l];
				}

				curVertex.litColor = brightness * curVertex.diffuseMaterial;
			}
		}
	}
	else if (lightType == LIGHTING_TYPE_SH_SHADOWED)
	{
		for (unsigned objIdx = 0; objIdx < nObjects; ++objIdx)
		{
			Object *curObject = scene.objects[objIdx];

			unsigned nVertices = curObject->vertices.size();

			for (unsigned verIdx = 0; verIdx < nVertices; ++verIdx)
			{
				Vertex &curVertex = curObject->vertices[verIdx];

				vec3 brightness = vec3(0.0f, 0.0f, 0.0f);

				unsigned nFunctions = light.nBands * light.nBands;
				for (unsigned l = 0; l < nFunctions; ++l)
				{
					brightness.r += light.rotatedCoeffs[0][l] * curVertex.shadowedCoeffs[l];
					brightness.g += light.rotatedCoeffs[1][l] * curVertex.shadowedCoeffs[l];
					brightness.b += light.rotatedCoeffs[2][l] * curVertex.shadowedCoeffs[l];
				}

				curVertex.litColor = brightness * curVertex.diffuseMaterial;
			}
		}
	}
}

void PRTRenderer::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key) {
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	/*case GLFW_KEY_SPACE:
		if (action == GLFW_PRESS) {
			cout << "set animation\n" << endl;
			bAnim = !bAnim;
		}
		break;*/
	case GLFW_KEY_1:
		lightType = LIGHTING_TYPE_SH_UNSHADOWED;
		break;
	case GLFW_KEY_2:
		lightType = LIGHTING_TYPE_SH_SHADOWED;
		break;
	}
}

void PRTRenderer::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button != GLFW_MOUSE_BUTTON_LEFT)return;
	if (action == GLFW_PRESS) {
		isLeftButtonPress = true;
	}
	else {
		isLeftButtonPress = false;
	}
}

void PRTRenderer::cursorPositionCallback(GLFWwindow* window, double x, double y)
{
	theta += ((y - preMouseY) * 0.05f);
	if (theta < 0.0f)theta = 0.0f;
	if (theta > 180.0f)theta = 180.0f;
	phi += ((x - preMouseX) * 0.05f);

	preMouseX = x;
	preMouseY = y;

	isNeedRotate = true;
}

PRTRenderer::~PRTRenderer()
{

}