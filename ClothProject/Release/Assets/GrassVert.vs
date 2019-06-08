#version 330 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 aColor;

out VS_OUT{
	out vec3 bColor;
}vs_out;

void main(void)
{
	vs_out.bColor = aColor;
	gl_Position = vec4(vertex, 1.0);
}