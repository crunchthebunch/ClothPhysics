#version 430 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 color;

out VS_GS_VERTEX{
	out vec4 vertex;
	out vec3 color;
	out mat4 mvp;
}vs_out;

uniform mat4 mvp;

void main(void)
{
	gl_Position = mvp * vec4(vertex, 1.0);
	
	vs_out.color = color;
	vs_out.vertex = gl_Position;
	vs_out.mvp = mvp;
}