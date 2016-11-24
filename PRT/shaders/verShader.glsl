#version 430 core

in vec3 position;
in vec3 color;

uniform mat4 MVP;

out vec3 fcolor;

void main()
{
	gl_Position = MVP * vec4(position, 1.0);
	fcolor = color;
}