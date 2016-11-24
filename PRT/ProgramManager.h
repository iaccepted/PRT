/***************************************
*author: guohongzhi  zju
*date:2015.8.23
*func: program manager
****************************************/
#ifndef _PROGRAMMANAGER_H_
#define _PROGRAMMANAGER_H_

#include "Global.h"

namespace GLSLShader
{
	typedef enum
	{
		VERTEX = 0, FRAGMENT, GEOMETRY,
		TESS_CONTROL, TESS_EVALUATION
	}shaderType;
};

class ProgramManager
{
public:
	ProgramManager();

	bool compileShaderFromFile(const char *fileName, GLSLShader::shaderType type);
	bool compileShaderFromString(const char *source, GLSLShader::shaderType type);
	bool link();
	bool validate();
	void use();
	void deleteProgram();
	string log();
	GLuint getHandle();
	bool isLinked();

	void   bindAttribLocation(GLuint location, const char *name);
	void   bindFragDataLocation(GLuint location, const char *name);

	void   setUniform(const char *name, float x, float y, float z);
	void   setUniform(const char *name, const vec2 &v);
	void   setUniform(const char *name, const vec3 &v);
	void   setUniform(const char *name, const vec4 &v);
	void   setUniform(const char *name, const mat4 &m);
	void   setUniform(const char *name, const mat3 &m);
	void   setUniform(const char *name, float val);
	void   setUniform(const char *name, int val);
	void   setUniform(const char *name, bool val);

	void   printActiveUniforms();
	void   printActiveAttribs();

private:
	GLuint handle;
	bool linked;
	string logString;

	bool fileExits(const char *fileName);
	const char *shaderSourceFromFile(const char *fileName);
	GLuint getUniformLocation(const char *name);
};

#endif