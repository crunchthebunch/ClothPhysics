#version 450 core

layout (location = 0) in vec3 vertex;

out vec3 fragTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main(void)
{
	gl_Position = proj * view * model * vec4(vertex, 1.0);
	fragTexCoord = vertex;
}