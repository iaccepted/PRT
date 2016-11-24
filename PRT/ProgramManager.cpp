#include "ProgramManager.h"
#include <sys/stat.h>
#include <stdio.h>

ProgramManager::ProgramManager() :handle(0), linked(false){}

bool ProgramManager::compileShaderFromFile(const char *fileName, GLSLShader::shaderType type)
{
	if (!this->fileExits(fileName))
	{
		//cerr << "File Not Exist: " << fileName << endl;
		logString = "File Not Exist: " + string(fileName);
		return false;
	}

	const char *source = shaderSourceFromFile(fileName);
	return compileShaderFromString(source, type);
}

bool ProgramManager::compileShaderFromString(const char *source, GLSLShader::shaderType type)
{
	if (handle <= 0)
	{
		handle = glCreateProgram();
		if (handle == 0)
		{
			logString = "Failed to create program.";
			return false;
		}
	}

	GLuint shaderHandle = 0;
	switch (type)
	{
	case GLSLShader::VERTEX:
		shaderHandle = glCreateShader(GL_VERTEX_SHADER);
		break;
	case GLSLShader::FRAGMENT:
		shaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
		break;
	case GLSLShader::GEOMETRY:
		shaderHandle = glCreateShader(GL_GEOMETRY_SHADER);
		break;
	case GLSLShader::TESS_CONTROL:
		shaderHandle = glCreateShader(GL_TESS_CONTROL_SHADER);
		break;
	case GLSLShader::TESS_EVALUATION:
		shaderHandle = glCreateShader(GL_TESS_EVALUATION_SHADER);
		break;
	default:
		logString = "Shader type is error.";
		return false;
	}

	glShaderSource(shaderHandle, 1, &source, NULL);
	glCompileShader(shaderHandle);

	int stat;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &stat);
	if (GL_FALSE == stat)
	{
		int length = 0;
		logString = "";
		glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &length);
		if (length > 0)
		{
			char *c_log = new char[length + 1];
			glGetShaderInfoLog(shaderHandle, length, NULL, c_log);
			c_log[length] = '\0';
			logString = c_log;
			delete[] c_log;
		}
		return false;
	}
	glAttachShader(handle, shaderHandle);
	glDeleteShader(shaderHandle);
	return true;
}

bool ProgramManager::link()
{
	if (linked) return true;
	if (handle <= 0) return false;

	glLinkProgram(handle);

	int status = 0;
	glGetProgramiv(handle, GL_LINK_STATUS, &status);
	if (GL_FALSE == status) {
		int length = 0;
		logString = "";

		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);

		if (length > 0) {
			char * c_log = new char[length + 1];
			int written = 0;
			glGetProgramInfoLog(handle, length, &written, c_log);
			c_log[length] = '\0';
			logString = c_log;
			delete[] c_log;
		}

		return false;
	}
	linked = true;
	return true;
}

void ProgramManager::deleteProgram()
{
	glDeleteProgram(handle);
}

GLuint ProgramManager::getUniformLocation(const char *name)
{
	GLuint loc = glGetUniformLocation(handle, name);
	return loc;
}

void ProgramManager::setUniform(const char *name, float x, float y, float z)
{
	int loc = getUniformLocation(name);
	if (loc >= 0) {
		glUniform3f(loc, x, y, z);
	}
	else {
		cerr << "Uniform: " << name << " is not found." << endl;
	}
}

void ProgramManager::setUniform(const char *name, const vec3 & v)
{
	this->setUniform(name, v.x, v.y, v.z);
}

void ProgramManager::setUniform(const char *name, const vec4 & v)
{
	int loc = getUniformLocation(name);
	if (loc >= 0) {
		glUniform4f(loc, v.x, v.y, v.z, v.w);
	}
	else {
		cerr << "Uniform: " << name << " is not found." << endl;
	}
}

void ProgramManager::setUniform(const char *name, const vec2 & v)
{
	int loc = getUniformLocation(name);
	if (loc >= 0) {
		glUniform2f(loc, v.x, v.y);
	}
	else {
		cerr << "Uniform: " << name << " is not found." << endl;
	}
}

void ProgramManager::setUniform(const char *name, const mat4 & m)
{
	int loc = getUniformLocation(name);
	if (loc >= 0)
	{
		glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
	}
	else {
		cerr << "Uniform: " << name << " is not found." << endl;
	}
}

void ProgramManager::setUniform(const char *name, const mat3 & m)
{
	int loc = getUniformLocation(name);
	if (loc >= 0)
	{
		glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]);
	}
	else {
		cerr << "Uniform: " << name << " is not found." << endl;
	}
}

void ProgramManager::setUniform(const char *name, float val)
{
	int loc = getUniformLocation(name);
	if (loc >= 0)
	{
		glUniform1f(loc, val);
	}
	else {
		cerr << "Uniform: " << name << " is not found." << endl;
	}
}

void ProgramManager::setUniform(const char *name, int val)
{
	int loc = getUniformLocation(name);
	if (loc >= 0)
	{
		glUniform1i(loc, val);
	}
	else {
		cerr << "Uniform: " << name << " is not found." << endl;
	}
}

void ProgramManager::setUniform(const char *name, bool val)
{
	int loc = getUniformLocation(name);
	if (loc >= 0)
	{
		glUniform1i(loc, val);
	}
	else {
		cerr << "Uniform: " << name << " is not found." << endl;
	}
}

void ProgramManager::bindAttribLocation(GLuint location, const char *attribName)
{
	glBindAttribLocation(this->handle, location, attribName);
}

void ProgramManager::printActiveUniforms() {

	GLint nUniforms, size, location, maxLen;
	GLchar * name;
	GLsizei written;
	GLenum type;

	glGetProgramiv(handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen);
	glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &nUniforms);

	name = (GLchar *)malloc(maxLen);

	printf(" Location | Name\n");
	printf("------------------------------------------------\n");
	for (int i = 0; i < nUniforms; ++i) {
		glGetActiveUniform(handle, i, maxLen, &written, &size, &type, name);
		location = glGetUniformLocation(handle, name);
		printf(" %-8d | %s\n", location, name);
	}

	free(name);
}

void ProgramManager::printActiveAttribs() {

	GLint written, size, location, maxLength, nAttribs;
	GLenum type;
	GLchar * name;

	glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);
	glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTES, &nAttribs);

	name = (GLchar *)malloc(maxLength);

	printf(" Index | Name\n");
	printf("------------------------------------------------\n");
	for (int i = 0; i < nAttribs; i++) {
		glGetActiveAttrib(handle, i, maxLength, &written, &size, &type, name);
		location = glGetAttribLocation(handle, name);
		printf(" %-5d | %s\n", location, name);
	}

	free(name);
}


GLuint ProgramManager::getHandle()
{
	return handle;
}

bool ProgramManager::isLinked()
{
	return linked;
}

bool ProgramManager::validate()
{
	if (!isLinked()) return false;

	GLint status;
	glValidateProgram(handle);
	glGetProgramiv(handle, GL_VALIDATE_STATUS, &status);

	if (GL_FALSE == status) {
		int length = 0;
		logString = "";

		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);

		if (length > 0) {
			char * c_log = new char[length + 1];
			int written = 0;
			glGetProgramInfoLog(handle, length, &written, c_log);
			c_log[length] = '\0';
			logString = c_log;
			delete[] c_log;
		}
		return false;
	}
	return true;
}

void ProgramManager::use()
{
	if (handle <= 0 || (!linked)) return;
	glUseProgram(handle);
}

string ProgramManager::log()
{
	return logString;
}
bool ProgramManager::fileExits(const char *fileName)
{
	struct stat info;
	int ret = -1;

	ret = stat(fileName, &info);
	return 0 == ret;
}

const char *ProgramManager::shaderSourceFromFile(const char *fileName)
{
	FILE *in;

	in = fopen(fileName, "rb");
	fseek(in, 0, SEEK_END);
	unsigned size = ftell(in);
	fseek(in, 0, SEEK_SET);

	char *source = new char[size + 1];

	fread(source, sizeof(char), size, in);
	source[size] = '\0';
	fclose(in);

	return const_cast<const char *>(source);
}