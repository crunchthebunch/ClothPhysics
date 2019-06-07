#version 450 core

in vec3 fragColor;
in vec3 fragVertex;

out vec4 color;

void main(void)
{
	color = vec4(fragColor.x,fragColor.y,fragColor.z,1.0);
}