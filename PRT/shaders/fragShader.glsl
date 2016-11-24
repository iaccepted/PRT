#version 430 core

in vec3 fcolor;
out vec4 ocolor;

void main()
{
	ocolor = vec4(fcolor, 1.0);
}