#version 450 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

out vec3 fragColor;
out vec3 fragVertex;
out vec2 fragTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec2 texScale;
uniform vec2 texOffset;

void main(void)
{
	gl_Position = proj * view * model * vec4(vertex, 1.0);
	fragVertex = vec3(model * vec4(vertex, 1.0f));
	fragColor = color;
	fragTexCoord = (texCoord + texOffset) * texScale;
}